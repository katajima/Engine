#include "EnemyAttackSlotSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <DirectXGame/engine/Math/MathFunctions.h>
#include <limits>
#include <numbers>
#include <cmath>
#include <unordered_set>

namespace Character {

	void EnemyAttackSlotSystem::Initialize() {
		// 起動時は前フレームの担当者が存在しないので、全スロットを未使用にする
		ClearOwners();
	}

	void EnemyAttackSlotSystem::Update(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float targetRotateY,
		float dt
	) {
		// 敵リストから外れた保持情報を消して、古いポインタ参照を避ける
		CleanupSlotHolds(enemies);
		// ターゲット位置と向きに合わせて攻撃位置を作り直す
		CreateSlots(targetPos, targetRotateY);
		// 作り直したスロットへ、今フレームの担当者を再割り当てする
		ClearOwners();
		AssignSlots(enemies, dt);
	}

	void EnemyAttackSlotSystem::CreateSlots(const Vector3& targetPos, float targetRotateY) {
		// 全スロットを等間隔で円周上に置く
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
			// 位置は残し、使用者だけを毎フレームリセットする
			slot.owner = nullptr;
			slot.occupied = false;
		}
	}

	void EnemyAttackSlotSystem::AssignSlots(const std::vector<BaseEnemy*>& enemies, float dt) {
		for (BaseEnemy* enemy : enemies) {
			if (!enemy){
				continue;
			}
			// 群衆移動の役割が攻撃レイヤーにいる敵だけをスロット割り当て対象にする
			const EnemyFlockingSteering* flockingSteering = enemy->GetEnemyAiSystem()->GetCrowdSystem()->FindSteering(enemy);
			if (!flockingSteering || flockingSteering->layer != EnemyCrowdLayer::Attack) {
				// 攻撃レイヤーにいない敵はスロットを割り当てない
				continue;
			}

			SlotHoldState& hold = slotHolds_[enemy];
			if (hold.timer > 0.0f) {
				// 切り替え抑制時間を減らし、0未満にならないよう丸める
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

			// 未使用スロットの中から、この敵に一番近い位置を探す
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
				// 前回位置との差が小さい時は補間し、大きく変わる時だけクールタイムを入れる
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
				// 所有者が一致したスロットをそのまま返す
				return &slot;
			}
		}

		return nullptr;
	}

	void EnemyAttackSlotSystem::CleanupSlotHolds(const std::vector<BaseEnemy*>& enemies) {
		// 現在生きている敵だけを集合にして、保持テーブルを掃除する
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
