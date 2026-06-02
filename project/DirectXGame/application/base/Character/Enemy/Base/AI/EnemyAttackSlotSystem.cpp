#include "EnemyAttackSlotSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <DirectXGame/engine/Math/MathFunctions.h>
#include <limits>
#include <numbers>
#include <cmath>
#include <unordered_set>

namespace Character {

	void EnemyAttackSlotSystem::Initialize() {
		ClearOwners();
	}

	void EnemyAttackSlotSystem::Update(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float targetRotateY,
		float dt
	) {
		CleanupSlotHolds(enemies);
		CreateSlots(targetPos, targetRotateY);
		ClearOwners();
		AssignSlots(enemies, dt);
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

	void EnemyAttackSlotSystem::AssignSlots(const std::vector<BaseEnemy*>& enemies, float dt) {
		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				continue;
			}

			SlotHoldState& hold = slotHolds_[enemy];
			if (hold.timer > 0.0f) {
				hold.timer = (std::max)(0.0f, hold.timer - dt);
			}

			if (hold.hasSlot && hold.timer > 0.0f && hold.index < kSlotCount && !slots_[hold.index].occupied) {
				// まだクールタイム中なら、前回決めた攻撃位置を保持する
				slots_[hold.index].position = hold.position;
				slots_[hold.index].owner = enemy;
				slots_[hold.index].occupied = true;
				continue;
			}

			float nearestDistance = std::numeric_limits<float>::max();
			AttackSlot* nearestSlot = nullptr;
			uint32_t nearestIndex = 0;

			Vector3 enemyPos = enemy->GetWorldPosition();

			for (uint32_t i = 0; i < kSlotCount; ++i) {
				AttackSlot& slot = slots_[i];
				if (slot.occupied) {
					continue;
				}

				Vector3 toSlot = Subtract(slot.position, enemyPos);
				float distance = toSlot.Length();

				if (distance < nearestDistance) {
					nearestDistance = distance;
					nearestSlot = &slot;
					nearestIndex = i;
				}
			}

			if (nearestSlot) {
				const float targetDiff = hold.hasSlot ? DistanceXZ(hold.position, nearestSlot->position) : slotSwitchDistance_ + 1.0f;
				if (hold.hasSlot && targetDiff <= slotSwitchDistance_) {
					hold.position = Vector3::Lerp(hold.position, nearestSlot->position, 0.12f);
				}
				else {
					hold.position = nearestSlot->position;
					hold.timer = slotChangeCooldown_;
				}
				hold.index = nearestIndex;
				hold.hasSlot = true;

				nearestSlot->position = hold.position;
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

	void EnemyAttackSlotSystem::CleanupSlotHolds(const std::vector<BaseEnemy*>& enemies) {
		std::unordered_set<BaseEnemy*> activeEnemies;
		for (BaseEnemy* enemy : enemies) {
			if (enemy) {
				activeEnemies.insert(enemy);
			}
		}

		for (auto it = slotHolds_.begin(); it != slotHolds_.end();) {
			if (!activeEnemies.contains(it->first)) {
				it = slotHolds_.erase(it);
			}
			else {
				++it;
			}
		}
	}
}
