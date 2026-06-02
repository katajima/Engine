#include "EnemyCrowdSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_set>

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
		CleanupTargetHolds(enemies);

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
		BuildFlockingSteering(enemies, targetPos, dt);
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

		const CrowdBehaviorSettings& groupBehavior = groupEnemies.front()->GetCrowdBehavior();
		BaseEnemy* commander = nullptr;
		uint32_t commanderOrder = 0;
		if (groupBehavior.useCommander) {
			commanderOrder = (std::min)(groupBehavior.commanderOrder, static_cast<uint32_t>(groupEnemies.size() - 1));
			commander = groupEnemies[commanderOrder];
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
			slot.commander = commander;
			slot.groupId = groupId;
			slot.memberIndex = enemy->GetCrowdMemberIndex();
			slot.isCommander = commander && i == commanderOrder;
			slots_.push_back(slot);
		}
	}

	void EnemyCrowdSystem::BuildFlockingSteering(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float dt
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
			const CrowdSlot* slot = FindSlot(enemy);
			EnemyFlockingSteering steering = flockingSystem_.BuildSteering(
				enemy,
				enemies,
				targetPos,
				baseTarget,
				flowDirection,
				layer,
				slot ? slot->commander : nullptr,
				slot ? slot->isCommander : false
			);
			steerings_.push_back(ApplyTargetHold(enemy, steering, dt));
		}
	}

	EnemyFlockingSteering EnemyCrowdSystem::ApplyTargetHold(BaseEnemy* enemy, EnemyFlockingSteering steering, float dt) {
		if (!enemy) {
			return steering;
		}

		TargetHoldState& state = targetHolds_[enemy];
		if (state.timer > 0.0f) {
			state.timer = (std::max)(0.0f, state.timer - dt);
		}

		const CrowdBehaviorSettings& behavior = enemy->GetCrowdBehavior();
		const float cooldown = behavior.targetChangeCooldown > 0.0f ? behavior.targetChangeCooldown : defaultTargetHoldCooldown_;
		const float switchDistance = (std::max)(behavior.targetSwitchDistance, 0.1f);

		if (!state.hasTarget) {
			state.target = steering.moveTarget;
			state.direction = steering.flowDirection;
			state.timer = cooldown;
			state.hasTarget = true;
			return steering;
		}

		const float distance = DistanceXZ(state.target, steering.moveTarget);
		if (distance > switchDistance && state.timer > 0.0f) {
			// 接近・待機役は古い座標そのものではなく、前回決めた進行方向を保持する。
			// 絶対座標を固定すると、敵自身が移動した後も古い点へ戻ろうとして列が伸びやすい。
			if (steering.layer == EnemyCrowdLayer::Attack) {
				steering.moveTarget = state.target;
			}
			else if (state.direction.Length() > 0.001f) {
				const float lookAhead = (std::max)(DistanceXZ(enemy->GetWorldPosition(), steering.moveTarget), 1.0f);
				steering.moveTarget = enemy->GetWorldPosition() + state.direction.Normalize() * lookAhead;
				steering.moveTarget.y = state.target.y;
			}
			steering.isTargetHeld = true;
			return steering;
		}

		if (distance > switchDistance) {
			state.target = steering.moveTarget;
			state.direction = steering.flowDirection;
			state.timer = cooldown;
			return steering;
		}

		// 小さなずれは滑らかに追従させ、プレイヤー移動による自然な追従は残す
		state.target = Vector3::Lerp(state.target, steering.moveTarget, 0.12f);
		state.direction = Vector3::Lerp(state.direction, steering.flowDirection, 0.12f).Normalize();
		steering.moveTarget = state.target;
		return steering;
	}

	void EnemyCrowdSystem::CleanupTargetHolds(const std::vector<BaseEnemy*>& enemies) {
		std::unordered_set<BaseEnemy*> activeEnemies;
		for (BaseEnemy* enemy : enemies) {
			if (enemy) {
				activeEnemies.insert(enemy);
			}
		}

		for (auto it = targetHolds_.begin(); it != targetHolds_.end();) {
			if (!activeEnemies.contains(it->first)) {
				it = targetHolds_.erase(it);
			}
			else {
				++it;
			}
		}
	}
}
