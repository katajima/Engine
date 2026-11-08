#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace Engine {

	// 親イベントを持たないイベントを表すインデックス値。
	inline constexpr uint32_t kInvalidProfileEventIndex = std::numeric_limits<uint32_t>::max();

	/// <summary>
	/// CPU上で計測した1つのスコープの情報を保持する構造体。
	/// </summary>
	struct ProfileEvent {
		// ImGui表示や集計に使用するスコープ名。
		std::string name{};
		// 計測時計のエポックからの開始時刻（マイクロ秒）。
		uint64_t startTime = 0;
		// 計測時計のエポックからの終了時刻（マイクロ秒）。
		uint64_t endTime = 0;
		// このイベントを記録したOSスレッドID。
		uint32_t threadId = 0;
		// 階層表示用の深さ。
		uint32_t depth = 0;
		// 親イベントのインデックス。ルートイベントの場合は無効値。
		uint32_t parentIndex = kInvalidProfileEventIndex;

		/// <summary>
		/// イベントの実行時間をミリ秒で取得する。
		/// </summary>
		double GetDurationMilliseconds() const;
	};

	// 既存コードから参照される可能性がある旧名称を互換用に残す。
	using CpuProfileEvent = ProfileEvent;

	/// <summary>
	/// 1フレーム分のCPUプロファイル結果を保持する構造体。
	/// </summary>
	struct ProfileFrame {
		// プロファイラが割り当てた連番。
		uint64_t frameNumber = 0;
		// フレームの開始時刻（マイクロ秒）。
		uint64_t startTime = 0;
		// フレームの終了時刻（マイクロ秒）。
		uint64_t endTime = 0;
		// フレーム内で計測されたスコープ一覧。
		std::vector<ProfileEvent> events{};

		/// <summary>
		/// フレーム全体の実行時間をミリ秒で取得する。
		/// </summary>
		double GetDurationMilliseconds() const;
	};

	/// <summary>
	/// 保存済みフレームから算出した統計値を保持する構造体。
	/// </summary>
	struct ProfileStatistics {
		// 統計対象にしたフレーム数。
		size_t sampleCount = 0;
		// 平均フレーム時間（ミリ秒）。
		double averageFrameMilliseconds = 0.0;
		// 最短フレーム時間（ミリ秒）。
		double minimumFrameMilliseconds = 0.0;
		// 最長フレーム時間（ミリ秒）。
		double maximumFrameMilliseconds = 0.0;
		// 平均フレーム時間から算出したFPS。
		double framesPerSecond = 0.0;
	};
}
