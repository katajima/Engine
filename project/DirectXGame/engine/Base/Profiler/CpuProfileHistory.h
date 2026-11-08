#pragma once

#include "CpuProfileData.h"

#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <vector>

namespace Engine {

	/// <summary>
	/// CPUプロファイルの直近フレームをリングバッファ形式で管理するクラス。
	/// </summary>
	class CpuProfileHistory {
	public:
		/// <summary>
		/// 履歴管理を指定容量で初期化する。
		/// </summary>
		explicit CpuProfileHistory(size_t capacity = 240);

		/// <summary>
		/// 新しいフレームを履歴の末尾へ追加する。
		/// </summary>
		void AddFrame(ProfileFrame frame);

		/// <summary>
		/// 保存済みのフレームをすべて破棄する。
		/// </summary>
		void Clear();

		/// <summary>
		/// 保持する最大フレーム数を変更する。
		/// </summary>
		void SetCapacity(size_t capacity);

		/// <summary>
		/// 現在の最大保持数を取得する。
		/// </summary>
		size_t GetCapacity() const;

		/// <summary>
		/// 履歴をUIで安全に参照できるコピーとして取得する。
		/// </summary>
		std::vector<ProfileFrame> GetFrames() const;

		/// <summary>
		/// 最新フレームを取得する。履歴が空の場合は空のoptionalを返す。
		/// </summary>
		std::optional<ProfileFrame> GetLatestFrame() const;

		/// <summary>
		/// 直近フレームから統計値を算出する。
		/// </summary>
		ProfileStatistics CalculateStatistics(size_t sampleCount = 120) const;

	private:
		// 複数スレッドからの計測終了とUI参照を直列化するミューテックス。
		mutable std::mutex mutex_;
		// 古いフレームから順に削除する履歴コンテナ。
		std::deque<ProfileFrame> frames_{};
		// 履歴に保持するフレームの最大数。
		size_t capacity_ = 240;
	};
}
