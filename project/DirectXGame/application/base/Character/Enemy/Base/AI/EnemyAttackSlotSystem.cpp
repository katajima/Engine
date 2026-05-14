#include "EnemyAttackSlotSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <DirectXGame/engine/Math/MathFunctions.h>
#include <limits>
#include <numbers>
#include <cmath>

namespace Character {

	void EnemyAttackSlotSystem::Initialize() {
		ClearOwners();
	}

	void EnemyAttackSlotSystem::Update(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float targetRotateY
	) {
		CreateSlots(targetPos, targetRotateY);
		ClearOwners();
		AssignSlots(enemies);
	}

	void EnemyAttackSlotSystem::CreateSlots(const Vector3& targetPos, float targetRotateY) {
		const float angleStep = std::numbers::pi_v<float> *2.0f / static_cast<float>(kSlotCount);

		for (uint32_t i = 0; i < kSlotCount; ++i) {
			float angle = targetRotateY + angleStep * static_cast<float>(i);

			Vector3 offset{};
			offset.x = std::sin(angle) * slotRadius_;
			offset.y = 0.0f;
			offset.z = std::cos(angle) * slotRadius_;

			slots_[i].position = Add(targetPos, offset);
			slots_[i].occupied = false;
			slots_[i].owner = nullptr;
		}
	}

	void EnemyAttackSlotSystem::ClearOwners() {
		for (AttackSlot& slot : slots_) {
			slot.owner = nullptr;
			slot.occupied = false;
		}
	}

	void EnemyAttackSlotSystem::AssignSlots(const std::vector<BaseEnemy*>& enemies) {
		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				continue;
			}

			float nearestDistance = std::numeric_limits<float>::max();
			AttackSlot* nearestSlot = nullptr;

			Vector3 enemyPos = enemy->GetWorldPosition();

			for (AttackSlot& slot : slots_) {
				if (slot.occupied) {
					continue;
				}

				Vector3 toSlot = Subtract(slot.position, enemyPos);
				float distance = toSlot.Length();

				if (distance < nearestDistance) {
					nearestDistance = distance;
					nearestSlot = &slot;
				}
			}

			if (nearestSlot) {
				nearestSlot->owner = enemy;
				nearestSlot->occupied = true;
			}
		}
	}

	const AttackSlot* EnemyAttackSlotSystem::FindSlot(BaseEnemy* enemy) const {
		for (const AttackSlot& slot : slots_) {
			if (slot.owner == enemy) {
				return &slot;
			}
		}

		return nullptr;
	}
}