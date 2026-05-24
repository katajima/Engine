#pragma once
#include "EnemyCrowdData.h"
#include "EnemyCrowdDensitySystem.h"
#include <vector>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// 攻撃許可と密度判定から、各敵の移動役割を決定するシステム
	/// </summary>
	class EnemyCrowdLayerSystem {
	public:
		// Attack / Approach / Standby のうち現在使うレイヤーを返す
		EnemyCrowdLayer ResolveLayer(
			BaseEnemy* enemy,
			const std::vector<CrowdSlot>& slots,
			const Vector3& targetPos
		) const;

	private:
		EnemyCrowdDensitySystem densitySystem_;
	};
}
