#include "CpuProfilerWindow.h"

#ifdef _DEBUG
#include "imgui.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace Engine {
namespace {

	// 階層の深さに応じて、タイムライン上のイベント色を変える。
	ImU32 GetTimelineColor(uint32_t depth) {
		const float hue = std::fmod(0.08f + static_cast<float>(depth) * 0.13f, 1.0f);
		const ImVec4 color = ImColor::HSV(hue, 0.70f, 0.90f);
		return ImGui::ColorConvertFloat4ToU32(color);
	}

}

CpuProfilerWindow::CpuProfilerWindow(CpuProfiler& profiler) : profiler_(&profiler) {
	// 表示対象プロファイラの参照を保存し、毎フレーム同じ履歴を表示する。
}

void CpuProfilerWindow::Draw() {
	// ImGuiが未初期化のビルドでは、CPU計測本体だけを利用可能にする。
	if (profiler_ == nullptr || !isOpen_) {
		return;
	}

	// UIが計測結果をコピーしている間に履歴が変化しないよう、スナップショットを取得する。
	const std::vector<ProfileFrame> frames = profiler_->GetFrameHistory();
	if (!ImGui::Begin("CPU Profiler", &isOpen_)) {
		ImGui::End();
		return;
	}

	// 履歴が空でも設定項目は操作できるように、先に共通情報を描画する。
	DrawStatistics(frames);
	if (frames.empty()) {
		ImGui::TextUnformatted("No completed frame yet.");
		ImGui::End();
		return;
	}

	// 履歴が縮小された場合に、選択インデックスを末尾へ補正する。
	selectedFrameIndex_ = std::min(selectedFrameIndex_, frames.size() - 1);
	int selectedFrame = static_cast<int>(selectedFrameIndex_);
	if (ImGui::SliderInt("Frame", &selectedFrame, 0, static_cast<int>(frames.size() - 1))) {
		selectedFrameIndex_ = static_cast<size_t>(selectedFrame);
	}

	// 選択フレームの詳細を3つの表示領域へ分けて描画する。
	const ProfileFrame& frame = frames[selectedFrameIndex_];
	if (ImGui::CollapsingHeader("Hierarchy", ImGuiTreeNodeFlags_DefaultOpen)) {
		DrawHierarchy(frame);
	}
	if (ImGui::CollapsingHeader("Timeline", ImGuiTreeNodeFlags_DefaultOpen)) {
		DrawTimeline(frame);
	}
	if (ImGui::CollapsingHeader("Frame Graph", ImGuiTreeNodeFlags_DefaultOpen)) {
		DrawFrameGraph(frames);
	}

	ImGui::End();
}

bool CpuProfilerWindow::IsOpen() const {
	// 呼び出し側がメニュー表示状態を確認できるように開閉状態を返す。
	return isOpen_;
}

void CpuProfilerWindow::DrawStatistics(const std::vector<ProfileFrame>& frames) {
	// 計測有効状態をUIへ反映し、チェック変更をプロファイラへ通知する。
	bool enabled = profiler_->IsEnabled();
	if (ImGui::Checkbox("Enabled", &enabled)) {
		profiler_->SetEnabled(enabled);
	}

	// 履歴容量をUIから調整できるようにする。
	int historyCapacity = static_cast<int>(profiler_->GetHistoryCapacity());
	if (historyCapacity < 1) {
		historyCapacity = 1;
	}
	if (ImGui::SliderInt("History Capacity", &historyCapacity, 1, 1000)) {
		profiler_->SetHistoryCapacity(static_cast<size_t>(historyCapacity));
	}

	// 最新フレームを対象に統計情報を表示する。
	const ProfileStatistics statistics = profiler_->GetStatistics();
	ImGui::Text("Samples: %zu", statistics.sampleCount);
	ImGui::Text("Frame: %.3f ms (%.1f FPS)", statistics.averageFrameMilliseconds, statistics.framesPerSecond);
	ImGui::Text("Min / Max: %.3f / %.3f ms", statistics.minimumFrameMilliseconds, statistics.maximumFrameMilliseconds);
	if (ImGui::Button("Clear History")) {
		profiler_->ClearHistory();
	}
	ImGui::Separator();
	(void)frames;
}

void CpuProfilerWindow::DrawHierarchy(const ProfileFrame& frame) {
	// まず親を持たないイベントを描画し、子イベントは再帰関数へ委譲する。
	for (uint32_t eventIndex = 0; eventIndex < frame.events.size(); ++eventIndex) {
		if (frame.events[eventIndex].parentIndex == kInvalidProfileEventIndex) {
			DrawHierarchyEvent(frame, eventIndex);
		}
	}
}

void CpuProfilerWindow::DrawHierarchyEvent(const ProfileFrame& frame, uint32_t eventIndex) {
	// 不正な親参照があってもUIが停止しないよう、範囲外イベントは無視する。
	if (eventIndex >= frame.events.size()) {
		return;
	}

	const ProfileEvent& event = frame.events[eventIndex];
	const std::string label = event.name + "  [" + std::to_string(event.GetDurationMilliseconds()) + " ms]";
	ImGui::PushID(static_cast<int>(eventIndex));
	const bool hasChild = std::any_of(frame.events.begin(), frame.events.end(), [eventIndex](const ProfileEvent& child) {
		return child.parentIndex == eventIndex;
	});

	// 子を持つイベントはツリー、それ以外は葉として表示する。
	if (hasChild) {
		const bool isOpen = ImGui::TreeNode(label.c_str());
		if (isOpen) {
			for (uint32_t childIndex = 0; childIndex < frame.events.size(); ++childIndex) {
				if (frame.events[childIndex].parentIndex == eventIndex) {
					DrawHierarchyEvent(frame, childIndex);
				}
			}
			ImGui::TreePop();
		}
	} else {
		ImGui::BulletText("%s", label.c_str());
	}
	ImGui::PopID();
}

void CpuProfilerWindow::DrawTimeline(const ProfileFrame& frame) {
	// 描画領域とフレーム時間から、イベント時刻をピクセル位置へ変換する。
	const float width = std::max(ImGui::GetContentRegionAvail().x, 1.0f);
	const float rowHeight = 18.0f;
	const double totalMilliseconds = std::max(frame.GetDurationMilliseconds(), 0.001);
	const float height = std::max(rowHeight, rowHeight * static_cast<float>(frame.events.size()));
	const ImVec2 origin = ImGui::GetCursorScreenPos();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// 背景領域を確保して、後続のImGuiコントロールが重ならないようにする。
	ImGui::Dummy(ImVec2(width, height));
	for (size_t eventIndex = 0; eventIndex < frame.events.size(); ++eventIndex) {
		const ProfileEvent& event = frame.events[eventIndex];
		const double startMilliseconds = event.startTime > frame.startTime ? static_cast<double>(event.startTime - frame.startTime) / 1000.0 : 0.0;
		const double endMilliseconds = event.endTime > frame.startTime ? static_cast<double>(event.endTime - frame.startTime) / 1000.0 : startMilliseconds;
		const float left = origin.x + static_cast<float>(std::min(startMilliseconds / totalMilliseconds, 1.0) * width);
		const float right = origin.x + static_cast<float>(std::min(endMilliseconds / totalMilliseconds, 1.0) * width);
		const float top = origin.y + rowHeight * static_cast<float>(eventIndex);

		// つぶれたイベントも見えるよう、最低幅を1ピクセル確保する。
		drawList->AddRectFilled(ImVec2(left, top), ImVec2(std::max(right, left + 1.0f), top + rowHeight - 2.0f), GetTimelineColor(event.depth));
		if (right - left > 40.0f) {
			drawList->AddText(ImVec2(left + 3.0f, top + 1.0f), IM_COL32_WHITE, event.name.c_str());
		}
	}
}

void CpuProfilerWindow::DrawFrameGraph(const std::vector<ProfileFrame>& frames) {
	// PlotLinesへ渡す連続配列を履歴から構築する。
	std::vector<float> durations;
	durations.reserve(frames.size());
	float maximumMilliseconds = 1.0f;
	for (const ProfileFrame& frame : frames) {
		const float durationMilliseconds = static_cast<float>(frame.GetDurationMilliseconds());
		durations.push_back(durationMilliseconds);
		maximumMilliseconds = std::max(maximumMilliseconds, durationMilliseconds);
	}

	// 履歴が存在する場合だけフレームグラフを描画する。
	if (!durations.empty()) {
		ImGui::PlotLines("Frame Time (ms)", durations.data(), static_cast<int>(durations.size()), 0, nullptr, 0.0f, maximumMilliseconds * 1.1f, ImVec2(-1.0f, 100.0f));
	}
}
}
#else

namespace Engine {

CpuProfilerWindow::CpuProfilerWindow(CpuProfiler& profiler) : profiler_(&profiler) {
	// Releaseビルドでも型のライフサイクルを成立させる。
}

void CpuProfilerWindow::Draw() {
	// ImGui表示はデバッグビルドだけで行う。
}

bool CpuProfilerWindow::IsOpen() const {
	// Releaseビルドでは表示対象外のため、常に閉じていると扱う。
	return false;
}

void CpuProfilerWindow::DrawStatistics(const std::vector<ProfileFrame>&) {
	// ReleaseビルドではImGui表示を無効化する。
}

void CpuProfilerWindow::DrawHierarchy(const ProfileFrame&) {
	// ReleaseビルドではImGui表示を無効化する。
}

void CpuProfilerWindow::DrawHierarchyEvent(const ProfileFrame&, uint32_t) {
	// ReleaseビルドではImGui表示を無効化する。
}

void CpuProfilerWindow::DrawTimeline(const ProfileFrame&) {
	// ReleaseビルドではImGui表示を無効化する。
}

void CpuProfilerWindow::DrawFrameGraph(const std::vector<ProfileFrame>&) {
	// ReleaseビルドではImGui表示を無効化する。
}
}
#endif
