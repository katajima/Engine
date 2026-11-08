#include "CpuProfileHistory.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace Engine {

	CpuProfileHistory::CpuProfileHistory(size_t capacity) {
		// 0件では最新フレームも表示できないため、最低1件は保持する。
		capacity_ = std::max<size_t>(capacity, 1);
	}

	void CpuProfileHistory::AddFrame(ProfileFrame frame) {
		// 履歴の追加と古いデータの削除を同じロック内で行う。
		std::lock_guard<std::mutex> lock(mutex_);
		frames_.push_back(std::move(frame));

		// 設定された容量を超えた分だけ先頭から削除する。
		while (frames_.size() > capacity_) {
			frames_.pop_front();
		}
	}

	void CpuProfileHistory::Clear() {
		// UIが取得中のコピーと履歴の変更が競合しないように保護する。
		std::lock_guard<std::mutex> lock(mutex_);
		frames_.clear();
	}

	void CpuProfileHistory::SetCapacity(size_t capacity) {
		// 容量0を指定されても履歴機能が停止しないように補正する。
		std::lock_guard<std::mutex> lock(mutex_);
		capacity_ = std::max<size_t>(capacity, 1);

		// 縮小後の容量に収まるまで最も古いフレームを削除する。
		while (frames_.size() > capacity_) {
			frames_.pop_front();
		}
	}

	size_t CpuProfileHistory::GetCapacity() const {
		// 容量の読み取りも、UIと設定変更の競合を避けるために保護する。
		std::lock_guard<std::mutex> lock(mutex_);
		return capacity_;
	}

	std::vector<ProfileFrame> CpuProfileHistory::GetFrames() const {
		// dequeの内容をvectorへコピーし、ロックを保持したまま外部へ返さない。
		std::lock_guard<std::mutex> lock(mutex_);
		return std::vector<ProfileFrame>(frames_.begin(), frames_.end());
	}

	std::optional<ProfileFrame> CpuProfileHistory::GetLatestFrame() const {
		// 最新フレームが存在しない場合は空の結果を返す。
		std::lock_guard<std::mutex> lock(mutex_);
		if (frames_.empty()) {
			return std::nullopt;
		}

		// 履歴末尾のフレームをUI用にコピーする。
		return frames_.back();
	}

	ProfileStatistics CpuProfileHistory::CalculateStatistics(size_t sampleCount) const {
		// 統計中に履歴が更新されないよう、先に一貫したコピーを取得する。
		const std::vector<ProfileFrame> frames = GetFrames();
		ProfileStatistics statistics{};
		if (frames.empty() || sampleCount == 0) {
			return statistics;
		}

		// 指定件数を超える場合は最新側のフレームだけを集計する。
		const size_t firstIndex = frames.size() > sampleCount ? frames.size() - sampleCount : 0;
		const size_t count = frames.size() - firstIndex;
		statistics.sampleCount = count;
		statistics.minimumFrameMilliseconds = std::numeric_limits<double>::max();

		// 対象フレームの合計・最小・最大を一度の走査で求める。
		double totalMilliseconds = 0.0;
		for (size_t index = firstIndex; index < frames.size(); ++index) {
			const double durationMilliseconds = frames[index].GetDurationMilliseconds();
			totalMilliseconds += durationMilliseconds;
			statistics.minimumFrameMilliseconds = std::min(statistics.minimumFrameMilliseconds, durationMilliseconds);
			statistics.maximumFrameMilliseconds = std::max(statistics.maximumFrameMilliseconds, durationMilliseconds);
		}

		// 平均値とFPSは、0除算を避けて算出する。
		statistics.averageFrameMilliseconds = totalMilliseconds / static_cast<double>(count);
		if (statistics.averageFrameMilliseconds > 0.0) {
			statistics.framesPerSecond = 1000.0 / statistics.averageFrameMilliseconds;
		}
		return statistics;
	}
}
