#pragma once
#include "EnemyCrowdData.h"
#include <vector>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// プレイヤー周囲に集まりすぎた敵を後方待機へ回すための密度判定
	/// </summary>
	class EnemyCrowdDensitySystem {
	public:
		// 前方に許容数以上の敵がいる場合、対象の敵を待機対象とする
		bool ShouldStandby(
			BaseEnemy* enemy,
			const std::vector<CrowdSlot>& slots,
			const Vector3& targetPos
		) const;

	private:
		float densityRadius_ = 12.0f;	// 密集として数えるプレイヤーからの半径
		uint32_t densityLimit_ = 6;		// 半径内で前に配置できる敵数
	};
}
