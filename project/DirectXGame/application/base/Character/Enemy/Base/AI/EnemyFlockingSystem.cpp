#include "EnemyFlockingSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Move/Base/MoveComponent.h"

namespace Character {
	EnemyFlockingSteering EnemyFlockingSystem::BuildSteering(
		BaseEnemy* enemy,
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		const Vector3& baseTarget,
		const Vector3& flowDirection,
		EnemyCrowdLayer layer,
		BaseEnemy* commander,
		bool isCommander
	) const {
		EnemyFlockingSteering steering{};
		steering.owner = enemy;
		steering.layer = layer;
		steering.commander = commander;
		steering.isCommander = isCommander;
		steering.isDensityLimited = layer == EnemyCrowdLayer::Standby;

		if (!enemy) {
			return steering;
		}

		// 隊形上の目的地へ直接進む場合の基準方向を用意する
		Vector3 toTarget = baseTarget - enemy->GetWorldPosition();
		toTarget.y = 0.0f;
		Vector3 baseDirection = NormalizeXZ(toTarget);

		// 群れ行動の3要素へ、全体の進行方向であるFlow Fieldを合成する
		Vector3 combined =
			CalculateSeparation(enemy, enemies) * separationWeight_ +
			CalculateAlignment(enemy, enemies) * alignmentWeight_ +
			CalculateCohesion(enemy, enemies) * cohesionWeight_ +
			flowDirection * flowWeight_;

		if (combined.Length() <= 0.001f) {
			combined = baseDirection;
		}

		Vector3 direction = NormalizeXZ(combined);
		if (direction.Length() <= 0.001f) {
			direction = baseDirection;
		}

		if (commander && !isCommander && enemy->GetCrowdBehavior().type != EnemyCrowdBehaviorType::Formation) {
			// 統率者ありの群衆では、全員が完全に別々の判断へ散らばらないよう統率者側の流れを少し混ぜる。
			// 隊列型は隊形維持を優先し、統率者は攻撃順やデバッグ上の役割として扱う。
			Vector3 toCommander = commander->GetWorldPosition() - enemy->GetWorldPosition();
			toCommander.y = 0.0f;
			Vector3 commanderDirection = NormalizeXZ(toCommander);
			if (commanderDirection.Length() > 0.001f) {
				direction = NormalizeXZ(direction + commanderDirection * enemy->GetCrowdBehavior().commanderInfluenceWeight);
			}
		}

		if (layer == EnemyCrowdLayer::Attack) {
			// 攻撃役は回避によって攻撃位置から外れないよう、スロットを優先する
			direction = baseDirection;
			steering.moveTarget = baseTarget;
		}
		else {
			// 移動の揺れを抑えるため、進行方向上の近い地点を毎フレーム目標とする
			steering.moveTarget = enemy->GetWorldPosition() + direction * steeringLookAhead_;
			steering.moveTarget.y = baseTarget.y;

			if (layer == EnemyCrowdLayer::Standby) {
				// 待機役はプレイヤーから離れる側に位置をずらし、前列を空ける
				Vector3 awayFromTarget = NormalizeXZ(enemy->GetWorldPosition() - targetPos);
				if (awayFromTarget.Length() > 0.001f) {
					steering.moveTarget = baseTarget + awayFromTarget * standbyBackDistance_;
					steering.moveTarget.y = baseTarget.y;
				}
			}
		}

		steering.flowDirection = direction;
		return steering;
	}

	Vector3 EnemyFlockingSystem::CalculateSeparation(BaseEnemy* owner, const std::vector<BaseEnemy*>& enemies) const {
		Vector3 result{};
		if (!owner) {
			return result;
		}

		Vector3 ownerPos = owner->GetWorldPosition();
		// 近い敵ほど強く押し離し、敵同士の重なりを軽減する
		for (BaseEnemy* other : enemies) {
			if (!other || other == owner) {
				continue;
			}

			Vector3 diff = ownerPos - other->GetWorldPosition();
			diff.y = 0.0f;
			float distance = diff.Length();
			if (distance <= 0.001f || distance > separationRadius_) {
				continue;
			}

			result += diff.Normalize() * ((separationRadius_ - distance) / separationRadius_);
		}

		return NormalizeXZ(result);
	}

	Vector3 EnemyFlockingSystem::CalculateAlignment(BaseEnemy* owner, const std::vector<BaseEnemy*>& enemies) const {
		Vector3 average{};
		uint32_t count = 0;
		if (!owner) {
			return average;
		}

		Vector3 ownerPos = owner->GetWorldPosition();
		// 周辺の仲間が既に進んでいる方向を平均し、移動方向をそろえる
		for (BaseEnemy* other : enemies) {
			if (!other || other == owner || !other->GetMoveComponent()) {
				continue;
			}

			if (DistanceXZ(ownerPos, other->GetWorldPosition()) > alignmentRadius_) {
				continue;
			}

			Vector3 direction = other->GetMoveComponent()->GetDirection();
			direction.y = 0.0f;
			if (direction.Length() <= 0.001f) {
				continue;
			}

			average += direction.Normalize();
			count++;
		}

		if (count == 0) {
			return {};
		}

		return NormalizeXZ(average / static_cast<float>(count));
	}

	Vector3 EnemyFlockingSystem::CalculateCohesion(BaseEnemy* owner, const std::vector<BaseEnemy*>& enemies) const {
		Vector3 center{};
		uint32_t count = 0;
		if (!owner) {
			return center;
		}

		Vector3 ownerPos = owner->GetWorldPosition();
		// 別グループを混ぜず、同じ群れだけの中心を求める
		for (BaseEnemy* other : enemies) {
			if (!other || other == owner || other->GetCrowdGroupId() != owner->GetCrowdGroupId()) {
				continue;
			}

			if (DistanceXZ(ownerPos, other->GetWorldPosition()) > cohesionRadius_) {
				continue;
			}

			center += other->GetWorldPosition();
			count++;
		}

		if (count == 0) {
			return {};
		}

		center /= static_cast<float>(count);
		Vector3 toCenter = center - ownerPos;
		toCenter.y = 0.0f;
		return NormalizeXZ(toCenter);
	}

	Vector3 EnemyFlockingSystem::NormalizeXZ(const Vector3& value) const {
		Vector3 result = value;
		result.y = 0.0f;
		float length = result.Length();
		if (length <= 0.001f) {
			return {};
		}

		return result / length;
	}
}
