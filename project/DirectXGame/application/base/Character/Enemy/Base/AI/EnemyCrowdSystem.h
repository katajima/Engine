#pragma once
#include <DirectXGame/engine/struct/Vector3.h>
#include <cstdint>
#include <vector>

namespace Character {
	class BaseEnemy;

	// 群衆隊形で敵が目指す位置
	struct CrowdSlot {
		Vector3 position{};				// 隊形上の目標位置
		BaseEnemy* owner = nullptr;		// この位置を担当する敵
		int groupId = 0;					// 群衆グループID
		uint32_t memberIndex = 0;		// グループ内での並び順
	};

	/// <summary>
	/// 敵の群衆隊形を管理するシステム
	/// </summary>
	class EnemyCrowdSystem {
	public:
		// 初期化
		void Initialize();

		// 敵一覧からグループごとの隊形スロットを作成する
		void Update(
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos,
			float targetRotateY
		);

		// 指定した敵に割り当てられた隊形スロットを取得
		const CrowdSlot* FindSlot(BaseEnemy* enemy) const;

	private:
		// 1グループ分の隊形スロットを割り当てる
		void AssignGroupSlots(
			const std::vector<BaseEnemy*>& groupEnemies,
			int groupId,
			const Vector3& targetPos,
			const Vector3& forward,
			const Vector3& right,
			uint32_t groupOrder
		);

	private:
		std::vector<CrowdSlot> slots_;

		float frontDistance_ = 8.0f;		// ターゲットから隊形先頭までの距離
		float rowSpacing_ = 3.0f;			// 前後の間隔
		float columnSpacing_ = 2.6f;		// 左右の間隔
		uint32_t columnCount_ = 3;			// 1列に並ぶ敵数
	};
}
