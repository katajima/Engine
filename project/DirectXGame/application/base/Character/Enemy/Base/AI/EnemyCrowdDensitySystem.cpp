#include "EnemyCrowdDensitySystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <cmath>

namespace Character {
	bool EnemyCrowdDensitySystem::ShouldStandby(
		BaseEnemy* enemy,
		const std::vector<CrowdSlot>& slots,
		const Vector3& targetPos
	) const {
		if (!enemy) {
			return false;
		}

		const float ownerDistance = DistanceXZ(enemy->GetWorldPosition(), targetPos);
		uint32_t closerCount = 0;

		// 自分よりプレイヤーに近い敵だけを数え、後ろ側から待機へ送る
		for (const CrowdSlot& slot : slots) {
			if (!slot.owner) {
				continue;
			}

			float distance = DistanceXZ(slot.owner->GetWorldPosition(), targetPos);
			if (distance > densityRadius_) {
				continue;
			}

			if (distance < ownerDistance ||
				(std::abs(distance - ownerDistance) <= 0.001f &&
					slot.owner->GetCrowdMemberIndex() < enemy->GetCrowdMemberIndex())) {
				closerCount++;
			}
		}

		// 密集範囲の外にいる敵には、通常どおり接近を許可する
		return ownerDistance <= densityRadius_ && closerCount >= densityLimit_;
	}
}
