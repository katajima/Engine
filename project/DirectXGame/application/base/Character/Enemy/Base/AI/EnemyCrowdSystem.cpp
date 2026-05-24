#include "EnemyCrowdSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace Character {

	void EnemyCrowdSystem::Initialize() {
		slots_.clear();
		steerings_.clear();
		patternSystem_.Initialize();
	}

	void EnemyCrowdSystem::Update(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float targetRotateY,
		float dt
	) {
		slots_.clear();
		steerings_.clear();
		patternSystem_.Update(dt);

		// グループIDごとに敵を分け、グループ単位で隊形を組む
		std::map<int, std::vector<BaseEnemy*>> groups;
		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				continue;
			}

			groups[enemy->GetCrowdGroupId()].push_back(enemy);
		}

		// プレイヤーの向きを基準に、隊形の前後方向と左右方向を決める
		Vector3 forward{};
		forward.x = std::sin(targetRotateY);
		forward.y = 0.0f;
		forward.z = std::cos(targetRotateY);

		Vector3 right{};
		right.x = std::cos(targetRotateY);
		right.y = 0.0f;
		right.z = -std::sin(targetRotateY);

		uint32_t groupOrder = 0;
		for (auto& group : groups) {
			std::vector<BaseEnemy*>& groupEnemies = group.second;
			// 生成順を使って、毎フレーム同じ敵が同じ並び順になるようにする
			std::sort(groupEnemies.begin(), groupEnemies.end(),
				[](const BaseEnemy* a, const BaseEnemy* b) {
					return a->GetCrowdMemberIndex() < b->GetCrowdMemberIndex();
				});

			AssignGroupSlots(groupEnemies, group.first, targetPos, forward, right, groupOrder);
			groupOrder++;
		}

		// 隊形を基準に、各敵が今フレーム進むべき位置を計算する
		BuildFlockingSteering(enemies, targetPos);
	}

	const CrowdSlot* EnemyCrowdSystem::FindSlot(BaseEnemy* enemy) const {
		for (const CrowdSlot& slot : slots_) {
			if (slot.owner == enemy) {
				return &slot;
			}
		}

		return nullptr;
	}

	const EnemyFlockingSteering* EnemyCrowdSystem::FindSteering(BaseEnemy* enemy) const {
		for (const EnemyFlockingSteering& steering : steerings_) {
			if (steering.owner == enemy) {
				return &steering;
			}
		}

		return nullptr;
	}

	void EnemyCrowdSystem::AssignGroupSlots(
		const std::vector<BaseEnemy*>& groupEnemies,
		int groupId,
		const Vector3& targetPos,
		const Vector3& forward,
		const Vector3& right,
		uint32_t groupOrder
	) {
		if (groupEnemies.empty()) {
			return;
		}

		// 複数グループが同じ位置に重ならないよう、グループごとに少しずらす
		const float groupSideOffset = (static_cast<float>(groupOrder % 3) - 1.0f) * columnSpacing_ * 1.5f;
		const float groupBackOffset = static_cast<float>(groupOrder / 3) * rowSpacing_ * 1.5f;

		for (uint32_t i = 0; i < groupEnemies.size(); ++i) {
			BaseEnemy* enemy = groupEnemies[i];
			if (!enemy) {
				continue;
			}

			uint32_t row = i / columnCount_;
			uint32_t column = i % columnCount_;
			float centeredColumn = static_cast<float>(column) - (static_cast<float>(columnCount_ - 1) * 0.5f);

			// ターゲット後方に、横3列を基準とした隊形位置を作る
			Vector3 slotPos = targetPos;
			slotPos -= forward * (frontDistance_ + rowSpacing_ * static_cast<float>(row) + groupBackOffset);
			slotPos += right * (centeredColumn * columnSpacing_ + groupSideOffset);
			slotPos.y = enemy->GetWorldPosition().y;

			// グループの設定に従い、従来の隊形位置を各戦術の位置へ置き換える
			slotPos = patternSystem_.CalculateTarget(
				enemy->GetCrowdBehavior(),
				groupEnemies,
				i,
				targetPos,
				forward,
				right,
				slotPos
			);

			CrowdSlot slot{};
			slot.position = slotPos;
			slot.owner = enemy;
			slot.groupId = groupId;
			slot.memberIndex = enemy->GetCrowdMemberIndex();
			slots_.push_back(slot);
		}
	}

	void EnemyCrowdSystem::BuildFlockingSteering(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos
	) {
		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				continue;
			}

			Vector3 baseTarget = targetPos;
			if (const CrowdSlot* slot = FindSlot(enemy)) {
				baseTarget = slot->position;
			}

			// 役割に応じた移動制約と群れ行動を、個別のシステムで順に合成する
			EnemyCrowdLayer layer = layerSystem_.ResolveLayer(enemy, slots_, targetPos);
			Vector3 flowDirection = flowFieldSystem_.CalculateDirection(enemy, baseTarget);
			steerings_.push_back(
				flockingSystem_.BuildSteering(enemy, enemies, targetPos, baseTarget, flowDirection, layer)
			);
		}
	}
}
