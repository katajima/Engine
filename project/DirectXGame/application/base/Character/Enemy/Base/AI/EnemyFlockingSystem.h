#pragma once
#include "EnemyCrowdData.h"
#include <vector>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// Separation / Alignment / Cohesion を合成して群れの移動目標を作るシステム
	/// </summary>
	class EnemyFlockingSystem {
	public:
		// 流れ方向と群れ行動を合わせ、敵が実際に目指す位置を返す
		EnemyFlockingSteering BuildSteering(
			BaseEnemy* enemy,
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos,
			const Vector3& baseTarget,
			const Vector3& flowDirection,
			EnemyCrowdLayer layer,
			BaseEnemy* commander,
			bool isCommander
		) const;

	private:
		// 近すぎる仲間から離れる方向を求める
		Vector3 CalculateSeparation(BaseEnemy* owner, const std::vector<BaseEnemy*>& enemies) const;
		// 周囲の仲間の進行方向に合わせる方向を求める
		Vector3 CalculateAlignment(BaseEnemy* owner, const std::vector<BaseEnemy*>& enemies) const;
		// 同じ群れの中心へ寄る方向を求める
		Vector3 CalculateCohesion(BaseEnemy* owner, const std::vector<BaseEnemy*>& enemies) const;
		Vector3 NormalizeXZ(const Vector3& value) const;

	private:
		float separationRadius_ = 2.4f;	// 分離を適用する距離
		float alignmentRadius_ = 7.0f;		// 整列の参照範囲
		float cohesionRadius_ = 9.0f;		// 結合の参照範囲

		float separationWeight_ = 3.2f;	// 重なり回避を強く優先する
		float alignmentWeight_ = 0.8f;
		float cohesionWeight_ = 0.9f;
		float flowWeight_ = 1.6f;			// 隊形の目標方向へ進む強さ
		float steeringLookAhead_ = 5.0f;	// 進行方向上に置く仮の目標距離
		float standbyBackDistance_ = 8.0f;	// 密集時に後ろへ下げる距離
	};
}
