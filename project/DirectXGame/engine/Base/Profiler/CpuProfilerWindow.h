#pragma once

#include "CpuProfiler.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Engine {

	/// <summary>
	/// CPUプロファイルの統計・階層・タイムライン・フレームグラフをImGuiへ描画するクラス。
	/// </summary>
	class CpuProfilerWindow {
	public:
		/// <summary>
		/// 指定されたプロファイラを表示対象として初期化する。
		/// </summary>
		explicit CpuProfilerWindow(CpuProfiler& profiler = CpuProfiler::Get());

		/// <summary>
		/// プロファイルウィンドウを1フレーム分描画する。
		/// </summary>
		void Draw();

		/// <summary>
		/// ウィンドウが表示状態かどうかを取得する。
		/// </summary>
		bool IsOpen() const;

	private:
		/// <summary>
		/// 統計値と履歴設定を描画する。
		/// </summary>
		void DrawStatistics(const std::vector<ProfileFrame>& frames);

		/// <summary>
		/// 選択フレームのイベント階層を描画する。
		/// </summary>
		void DrawHierarchy(const ProfileFrame& frame);

		/// <summary>
		/// イベントを再帰的に描画する。
		/// </summary>
		void DrawHierarchyEvent(const ProfileFrame& frame, uint32_t eventIndex);

		/// <summary>
		/// 選択フレームの時間軸を描画する。
		/// </summary>
		void DrawTimeline(const ProfileFrame& frame);

		/// <summary>
		/// 履歴のフレーム時間グラフを描画する。
		/// </summary>
		void DrawFrameGraph(const std::vector<ProfileFrame>& frames);

		// 実際に計測データを供給するプロファイラへの参照。
		CpuProfiler* profiler_ = nullptr;
		// ImGui上で現在表示しているフレームの履歴内インデックス。
		size_t selectedFrameIndex_ = 0;
		// ウィンドウの開閉状態。
		bool isOpen_ = true;
	};
}
