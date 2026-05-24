#include "EnemyCrowdPatternSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace Character {
	void EnemyCrowdPatternSystem::Initialize() {
		elapsedTime_ = 0.0f;
	}

	void EnemyCrowdPatternSystem::Update(float dt) {
		elapsedTime_ += dt;
	}

	Vector3 EnemyCrowdPatternSystem::CalculateTarget(
		const CrowdBehaviorSettings& behavior,
		const std::vector<BaseEnemy*>& groupEnemies,
		uint32_t memberIndex,
		const Vector3& targetPos,
		const Vector3& forward,
		const Vector3& right,
		const Vector3& fallbackPos
	) const {
		const uint32_t memberCount = static_cast<uint32_t>(groupEnemies.size());
		Vector3 result = fallbackPos;

		switch (behavior.type) {
		case EnemyCrowdBehaviorType::Rush:
			result = CalculateRushTarget(behavior, memberIndex, targetPos, right);
			break;
		case EnemyCrowdBehaviorType::Encircle:
			result = CalculateEncircleTarget(behavior, memberIndex, memberCount, targetPos);
			break;
		case EnemyCrowdBehaviorType::WaveAssault:
			result = CalculateWaveTarget(behavior, memberIndex, targetPos, forward, right);
			break;
		case EnemyCrowdBehaviorType::Formation:
			result = CalculateFormationTarget(behavior, memberIndex, memberCount, targetPos, forward, right);
			break;
		case EnemyCrowdBehaviorType::FlowCrowd:
			result = CalculateFlowTarget(behavior, memberIndex, memberCount, targetPos);
			break;
		case EnemyCrowdBehaviorType::PressureCrowd:
			result = CalculatePressureTarget(behavior, memberIndex, memberCount, targetPos);
			break;
		case EnemyCrowdBehaviorType::DistributedEncircle:
			result = CalculateDistributedTarget(behavior, memberIndex, memberCount, targetPos, forward);
			break;
		case EnemyCrowdBehaviorType::DensityAdaptive:
			// 少人数は突撃、中人数は包囲、大人数は密度を上げて圧迫する
			if (memberCount <= 4) {
				result = CalculateRushTarget(behavior, memberIndex, targetPos, right);
			}
			else if (memberCount <= 10) {
				result = CalculateEncircleTarget(behavior, memberIndex, memberCount, targetPos);
			}
			else {
				result = CalculatePressureTarget(behavior, memberIndex, memberCount, targetPos);
			}
			break;
		case EnemyCrowdBehaviorType::Spiral:
			result = CalculateSpiralTarget(behavior, memberIndex, memberCount, targetPos);
			break;
		case EnemyCrowdBehaviorType::Hunting:
			result = CalculateHuntingTarget(memberIndex, targetPos, forward, right);
			break;
		case EnemyCrowdBehaviorType::Flocking:
		default:
			// 群れ型は従来の隊形目標を使い、Flocking Systemで自然に補正する
			break;
		}

		if (memberIndex < groupEnemies.size() && groupEnemies[memberIndex]) {
			result.y = groupEnemies[memberIndex]->GetWorldPosition().y;
		}
		return result;
	}

	Vector3 EnemyCrowdPatternSystem::CalculateRushTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, const Vector3& targetPos, const Vector3& right
	) const {
		// 完全な一点集中を避けるため、わずかな横幅だけを残して突撃させる
		float lane = (static_cast<float>(memberIndex % 3) - 1.0f) * behavior.spacing * 0.5f;
		return targetPos + right * lane;
	}

	Vector3 EnemyCrowdPatternSystem::CalculateEncircleTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, uint32_t memberCount, const Vector3& targetPos
	) const {
		float step = std::numbers::pi_v<float> * 2.0f / static_cast<float>(std::max(memberCount, 1u));
		return CalculateRingPosition(targetPos, behavior.radius, step * static_cast<float>(memberIndex));
	}

	Vector3 EnemyCrowdPatternSystem::CalculateWaveTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, const Vector3& targetPos,
		const Vector3& forward, const Vector3& right
	) const {
		uint32_t waves = std::max(behavior.waveCount, 1u);
		uint32_t row = memberIndex % waves;
		uint32_t activeRow = static_cast<uint32_t>(elapsedTime_ / std::max(behavior.waveInterval, 0.1f)) % waves;
		float distance = row == activeRow ? behavior.radius * 0.65f : behavior.radius + 6.0f;
		float side = (static_cast<float>(memberIndex / waves) - 1.0f) * behavior.spacing;
		return targetPos - forward * distance + right * side;
	}

	Vector3 EnemyCrowdPatternSystem::CalculateFormationTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, uint32_t memberCount,
		const Vector3& targetPos, const Vector3& forward, const Vector3& right
	) const {
		float center = (static_cast<float>(memberCount) - 1.0f) * 0.5f;
		switch (behavior.formation) {
		case EnemyFormationShape::Circle:
			return CalculateEncircleTarget(behavior, memberIndex, memberCount, targetPos);
		case EnemyFormationShape::VShape: {
			uint32_t rank = (memberIndex + 1) / 2;
			float sign = memberIndex == 0 ? 0.0f : (memberIndex % 2 == 1 ? -1.0f : 1.0f);
			return targetPos - forward * (behavior.radius + rank * behavior.spacing) +
				right * (sign * rank * behavior.spacing);
		}
		case EnemyFormationShape::Square: {
			uint32_t columns = static_cast<uint32_t>(std::ceil(std::sqrt(static_cast<float>(std::max(memberCount, 1u)))));
			uint32_t row = memberIndex / columns;
			uint32_t column = memberIndex % columns;
			float side = (static_cast<float>(column) - (static_cast<float>(columns) - 1.0f) * 0.5f) * behavior.spacing;
			return targetPos - forward * (behavior.radius + row * behavior.spacing) + right * side;
		}
		case EnemyFormationShape::Line:
		default:
			return targetPos - forward * behavior.radius + right * ((static_cast<float>(memberIndex) - center) * behavior.spacing);
		}
	}

	Vector3 EnemyCrowdPatternSystem::CalculateFlowTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, uint32_t memberCount, const Vector3& targetPos
	) const {
		float step = std::numbers::pi_v<float> * 2.0f / static_cast<float>(std::max(memberCount, 1u));
		float angle = step * static_cast<float>(memberIndex) + elapsedTime_ * behavior.angularSpeed;
		return CalculateRingPosition(targetPos, behavior.radius, angle);
	}

	Vector3 EnemyCrowdPatternSystem::CalculatePressureTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, uint32_t memberCount, const Vector3& targetPos
	) const {
		// 人数が増えても狭い輪を保ち、外周からプレイヤーへ圧をかける
		float ring = behavior.radius + static_cast<float>(memberIndex / 8) * 2.0f;
		float step = std::numbers::pi_v<float> * 2.0f / static_cast<float>(std::min(std::max(memberCount, 1u), 8u));
		return CalculateRingPosition(targetPos, ring, step * static_cast<float>(memberIndex % 8));
	}

	Vector3 EnemyCrowdPatternSystem::CalculateDistributedTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, uint32_t memberCount,
		const Vector3& targetPos, const Vector3& forward
	) const {
		// 前方側に開口部を残す270度の弧へ配置し、逃げ道を意図的に作る
		float baseAngle = std::atan2(forward.x, forward.z);
		float arc = std::numbers::pi_v<float> * 1.5f;
		float t = memberCount <= 1 ? 0.5f : static_cast<float>(memberIndex) / static_cast<float>(memberCount - 1);
		float angle = baseAngle + std::numbers::pi_v<float> * 0.25f + arc * t;
		return CalculateRingPosition(targetPos, behavior.radius, angle);
	}

	Vector3 EnemyCrowdPatternSystem::CalculateSpiralTarget(
		const CrowdBehaviorSettings& behavior, uint32_t memberIndex, uint32_t memberCount, const Vector3& targetPos
	) const {
		float step = std::numbers::pi_v<float> * 2.0f / static_cast<float>(std::max(memberCount, 1u));
		float angle = step * static_cast<float>(memberIndex) + elapsedTime_ * behavior.angularSpeed;
		float pulse = (std::sin(elapsedTime_ * 1.5f + static_cast<float>(memberIndex)) + 1.0f) * 2.0f;
		return CalculateRingPosition(targetPos, behavior.radius - pulse, angle);
	}

	Vector3 EnemyCrowdPatternSystem::CalculateHuntingTarget(
		uint32_t memberIndex, const Vector3& targetPos, const Vector3& forward, const Vector3& right
	) const {
		EnemyHuntingRole role = static_cast<EnemyHuntingRole>(memberIndex % 4);
		switch (role) {
		case EnemyHuntingRole::Chaser:
			return targetPos - forward * 7.0f;
		case EnemyHuntingRole::Flanker:
			return targetPos + right * (memberIndex % 8 < 4 ? -10.0f : 10.0f);
		case EnemyHuntingRole::Ambusher:
			return targetPos + forward * 11.0f;
		case EnemyHuntingRole::Blocker:
		default:
			return targetPos - forward * 5.0f + right * (memberIndex % 8 < 4 ? -6.0f : 6.0f);
		}
	}

	Vector3 EnemyCrowdPatternSystem::CalculateRingPosition(const Vector3& center, float radius, float angle) const {
		Vector3 position = center;
		position.x += std::sin(angle) * radius;
		position.z += std::cos(angle) * radius;
		return position;
	}
}
