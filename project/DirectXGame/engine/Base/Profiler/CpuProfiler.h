#pragma once

#include "CpuProfileData.h"
#include "CpuProfileHistory.h"

#include <cstddef>
#include <mutex>
#include <optional>
#include <string_view>
#include <vector>

namespace Engine {

	/// <summary>
	/// CPUスコープをフレーム単位で記録するプロファイラ。
	/// </summary>
	class CpuProfiler {
	public:
		/// <summary>
		/// アプリケーション全体で共有するプロファイラを取得する。
		/// </summary>
		static CpuProfiler& Get();

		// コピーによって計測状態が分裂しないよう、コピーを禁止する。
		CpuProfiler(const CpuProfiler&) = delete;
		CpuProfiler& operator=(const CpuProfiler&) = delete;

		/// <summary>
		/// 新しいフレームの記録を開始する。
		/// </summary>
		void BeginFrame();

		/// <summary>
		/// 現在フレームの記録を完了し、履歴へ追加する。
		/// </summary>
		void EndFrame();

		/// <summary>
		/// 名前付きCPUスコープの記録を開始する。
		/// </summary>
		void BeginScope(std::string_view name);

		/// <summary>
		/// 現在スレッドで最後に開始したCPUスコープを完了する。
		/// </summary>
		void EndScope();

		/// <summary>
		/// 計測の有効・無効を設定する。
		/// </summary>
		void SetEnabled(bool enabled);

		/// <summary>
		/// 計測が有効かどうかを取得する。
		/// </summary>
		bool IsEnabled() const;

		/// <summary>
		/// 現在フレームの記録中かどうかを取得する。
		/// </summary>
		bool IsFrameActive() const;

		/// <summary>
		/// 保存する履歴の最大フレーム数を設定する。
		/// </summary>
		void SetHistoryCapacity(size_t capacity);

		/// <summary>
		/// 保存する履歴の最大フレーム数を取得する。
		/// </summary>
		size_t GetHistoryCapacity() const;

		/// <summary>
		/// 履歴をクリアする。
		/// </summary>
		void ClearHistory();

		/// <summary>
		/// 履歴をUI参照用のコピーとして取得する。
		/// </summary>
		std::vector<ProfileFrame> GetFrameHistory() const;

		/// <summary>
		/// 最新フレームを取得する。
		/// </summary>
		std::optional<ProfileFrame> GetLatestFrame() const;

		/// <summary>
		/// 履歴から統計値を取得する。
		/// </summary>
		ProfileStatistics GetStatistics(size_t sampleCount = 120) const;

	private:
		/// <summary>
		/// プロファイラを既定設定で初期化する。
		/// </summary>
		CpuProfiler();

		/// <summary>
		/// 計測状態を終了する。
		/// </summary>
		~CpuProfiler() = default;

		// フレーム状態とイベント配列を保護するミューテックス。
		mutable std::mutex mutex_;
		// 直近のBeginFrameからEndFrameまでに記録しているフレーム。
		ProfileFrame currentFrame_{};
		// 現在フレームが記録中かを示すフラグ。
		bool frameActive_ = false;
		// 計測機能全体の有効状態。
		bool enabled_ = true;
		// 次に割り当てるフレーム番号。
		uint64_t nextFrameNumber_ = 0;
		// 完了したフレームを保持する履歴管理クラス。
		CpuProfileHistory history_{};
	};

	// 既存の大文字クラス名を使うコードとの互換用別名。
	using CPUProfiler = CpuProfiler;
}
