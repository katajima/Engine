#include "CpuProfiler.h"

#include <Windows.h>

#include <algorithm>
#include <chrono>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace Engine {
namespace {

	// 1つのスレッドにおけるBeginScope/EndScopeの対応関係を保持するエントリ。
	struct ScopeEntry {
		// このスコープを開始したプロファイラ。
		CpuProfiler* profiler = nullptr;
		// 現在フレームのイベント配列内のインデックス。
		uint32_t eventIndex = kInvalidProfileEventIndex;
	};

	// スコープの入れ子関係はスレッドごとに独立しているため、TLSで保持する。
	thread_local std::vector<ScopeEntry> scopeStack;

	// steady_clockの時刻をプロファイルデータで扱うマイクロ秒へ変換する。
	uint64_t GetTimestampMicroseconds() {
		using Clock = std::chrono::steady_clock;
		using Microseconds = std::chrono::microseconds;
		return static_cast<uint64_t>(std::chrono::duration_cast<Microseconds>(Clock::now().time_since_epoch()).count());
	}

	// 現在のWindowsスレッドIDをUIで識別しやすい整数へ変換する。
	uint32_t GetCurrentThreadIdForProfile() {
		return static_cast<uint32_t>(::GetCurrentThreadId());
	}

}

CpuProfiler& CpuProfiler::Get() {
	// 静的ローカル変数により、初回利用時に一度だけ生成する。
	static CpuProfiler profiler;
	return profiler;
}

CpuProfiler::CpuProfiler() = default;

void CpuProfiler::BeginFrame() {
	// 前フレームが閉じられずに残った場合も、データを失わずに確定させる。
	if (IsFrameActive()) {
		EndFrame();
	}

	// 計測が無効な場合はフレームバッファを作らず、呼び出しコストを抑える。
	std::lock_guard<std::mutex> lock(mutex_);
	if (!enabled_) {
		frameActive_ = false;
		return;
	}

	// 新しいフレームの識別子と開始時刻を設定する。
	currentFrame_ = {};
	currentFrame_.frameNumber = nextFrameNumber_++;
	currentFrame_.startTime = GetTimestampMicroseconds();
	frameActive_ = true;
}

void CpuProfiler::EndFrame() {
	// 現在スレッドで閉じ忘れたスコープをフレーム終了時刻で補完する。
	const uint64_t endTime = GetTimestampMicroseconds();
	ProfileFrame finishedFrame{};
	{
		// フレームの終了・スコープ補完・移動を同じロックで行う。
		std::lock_guard<std::mutex> lock(mutex_);
		if (!frameActive_) {
			return;
		}
		currentFrame_.endTime = endTime;
		for (ScopeEntry& scopeEntry : scopeStack) {
			if (scopeEntry.profiler == this && scopeEntry.eventIndex != kInvalidProfileEventIndex && scopeEntry.eventIndex < currentFrame_.events.size()) {
				currentFrame_.events[scopeEntry.eventIndex].endTime = endTime;
			}
		}

		// 記録中のフレームを履歴追加用のローカル変数へ移動する。
		finishedFrame = std::move(currentFrame_);
		currentFrame_ = {};
		frameActive_ = false;
	}

	// 閉じ忘れたスコープが次フレームの親にならないよう、現在スレッドの対応情報を破棄する。
	scopeStack.erase(std::remove_if(scopeStack.begin(), scopeStack.end(), [this](const ScopeEntry& entry) {
		return entry.profiler == this;
	}), scopeStack.end());

	// ロックを解放してから履歴側のロックを取得し、ロック順序の逆転を防ぐ。
	history_.AddFrame(std::move(finishedFrame));
}

void CpuProfiler::BeginScope(std::string_view name) {
	// EndScopeと対になるエントリを先に積み、無効時でもスタックを壊さない。
	ScopeEntry scopeEntry{};
	scopeEntry.profiler = this;

	{
		// フレーム状態とイベント追加を同じロックで保護する。
		std::lock_guard<std::mutex> lock(mutex_);
		if (enabled_ && frameActive_ && currentFrame_.events.size() < kInvalidProfileEventIndex) {
			ProfileEvent event{};
			event.name = std::string(name);
			event.startTime = GetTimestampMicroseconds();
			event.threadId = GetCurrentThreadIdForProfile();

			// 同じプロファイラの直近イベントを親にして階層を構築する。
			for (auto iterator = scopeStack.rbegin(); iterator != scopeStack.rend(); ++iterator) {
				if (iterator->profiler == this && iterator->eventIndex != kInvalidProfileEventIndex && iterator->eventIndex < currentFrame_.events.size()) {
					event.parentIndex = iterator->eventIndex;
					event.depth = currentFrame_.events[iterator->eventIndex].depth + 1;
					break;
				}
			}

			// 追加したイベントをEndScopeから参照できるように記録する。
			currentFrame_.events.push_back(std::move(event));
			scopeEntry.eventIndex = static_cast<uint32_t>(currentFrame_.events.size() - 1);
		}
	}

	// イベントが無効でも対応するエントリを積み、RAIIの対称性を維持する。
	scopeStack.push_back(scopeEntry);
}

void CpuProfiler::EndScope() {
	// 対応するBeginScopeがない場合は不正な呼び出しとして無視する。
	if (scopeStack.empty()) {
		return;
	}

	// 最後に開始したスコープを取り出し、入れ子順を維持する。
	const ScopeEntry scopeEntry = scopeStack.back();
	scopeStack.pop_back();
	if (scopeEntry.profiler != this || scopeEntry.eventIndex == kInvalidProfileEventIndex) {
		return;
	}

	// イベントが終了した時点の時計を保存する。
	const uint64_t endTime = GetTimestampMicroseconds();
	std::lock_guard<std::mutex> lock(mutex_);
	if (frameActive_ && scopeEntry.eventIndex < currentFrame_.events.size()) {
		currentFrame_.events[scopeEntry.eventIndex].endTime = endTime;
	}
}

void CpuProfiler::SetEnabled(bool enabled) {
	// 有効状態の変更を計測開始・終了と競合させない。
	std::lock_guard<std::mutex> lock(mutex_);
	enabled_ = enabled;
}

bool CpuProfiler::IsEnabled() const {
	// UIからの読み取りを計測側の書き込みと同期する。
	std::lock_guard<std::mutex> lock(mutex_);
	return enabled_;
}

bool CpuProfiler::IsFrameActive() const {
	// フレーム状態の読み取りをミューテックスで保護する。
	std::lock_guard<std::mutex> lock(mutex_);
	return frameActive_;
}

void CpuProfiler::SetHistoryCapacity(size_t capacity) {
	// 容量変更の責務を履歴管理クラスへ集約する。
	history_.SetCapacity(capacity);
}

size_t CpuProfiler::GetHistoryCapacity() const {
	// UIが現在値を表示できるよう、履歴管理クラスの容量を返す。
	return history_.GetCapacity();
}

void CpuProfiler::ClearHistory() {
	// 完了済みフレームだけを削除し、現在計測中のフレームには触れない。
	history_.Clear();
}

std::vector<ProfileFrame> CpuProfiler::GetFrameHistory() const {
	// 履歴管理クラスが作成したスナップショットをそのまま返す。
	return history_.GetFrames();
}

std::optional<ProfileFrame> CpuProfiler::GetLatestFrame() const {
	// 最新フレームの取得処理を履歴管理クラスへ委譲する。
	return history_.GetLatestFrame();
}

ProfileStatistics CpuProfiler::GetStatistics(size_t sampleCount) const {
	// 統計の算出条件を履歴管理クラスへ委譲する。
	return history_.CalculateStatistics(sampleCount);
}
}
