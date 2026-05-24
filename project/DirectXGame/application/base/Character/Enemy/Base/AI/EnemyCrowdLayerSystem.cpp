#include "EnemyCrowdLayerSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAttackSystem.h"

namespace Character {
	EnemyCrowdLayer EnemyCrowdLayerSystem::ResolveLayer(
		BaseEnemy* enemy,
		const std::vector<CrowdSlot>& slots,
		const Vector3& targetPos
	) const {
		if (!enemy) {
			return EnemyCrowdLayer::Approach;
		}

		// Token Systemで選ばれた敵は、密度より優先して攻撃役にする
		EnemyAttackSystem* attackSystem = enemy->GetEnemyAttackSystem();
		if (attackSystem && attackSystem->GetAttackRequest()) {
			EnemyAttackRing ring = attackSystem->GetAttackRequest()->GetRing();
			if (ring == EnemyAttackRing::Attack) {
				return EnemyCrowdLayer::Attack;
			}
			if (ring == EnemyAttackRing::Standby || ring == EnemyAttackRing::TooClose) {
				return EnemyCrowdLayer::Standby;
			}
		}

		// 攻撃役でない敵は、密集時に後方待機へ振り分ける
		if (densitySystem_.ShouldStandby(enemy, slots, targetPos)) {
			return EnemyCrowdLayer::Standby;
		}

		return EnemyCrowdLayer::Approach;
	}
}
