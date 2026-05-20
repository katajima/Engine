#include "EnemyCrowdSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace Character {

	void EnemyCrowdSystem::Initialize() {
		slots_.clear();
	}

	void EnemyCrowdSystem::Update(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float targetRotateY
	) {
		slots_.clear();

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
	}

	const CrowdSlot* EnemyCrowdSystem::FindSlot(BaseEnemy* enemy) const {
		for (const CrowdSlot& slot : slots_) {
			if (slot.owner == enemy) {
				return &slot;
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

			CrowdSlot slot{};
			slot.position = slotPos;
			slot.owner = enemy;
			slot.groupId = groupId;
			slot.memberIndex = enemy->GetCrowdMemberIndex();
			slots_.push_back(slot);
		}
	}
}
