#pragma once
#include "EnemyCrowdBehaviorData.h"
#include <DirectXGame/engine/struct/Vector3.h>
#include <vector>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// 群衆タイプに応じて、各メンバーが目指す戦術上の位置を計算するシステム
	/// </summary>
	class EnemyCrowdPatternSystem {
	public:
		void Initialize();
		void Update(float dt);

		Vector3 CalculateTarget(
			const CrowdBehaviorSettings& behavior,
			const std::vector<BaseEnemy*>& groupEnemies,
			uint32_t memberIndex,
			const Vector3& targetPos,
			const Vector3& forward,
			const Vector3& right,
			const Vector3& fallbackPos
		) const;

	private:
		Vector3 CalculateRushTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			const Vector3& targetPos, const Vector3& right) const;
		Vector3 CalculateEncircleTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			uint32_t memberCount, const Vector3& targetPos) const;
		Vector3 CalculateWaveTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			const Vector3& targetPos, const Vector3& forward, const Vector3& right) const;
		Vector3 CalculateFormationTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			uint32_t memberCount, const Vector3& targetPos, const Vector3& forward, const Vector3& right) const;
		Vector3 CalculateFlowTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			uint32_t memberCount, const Vector3& targetPos) const;
		Vector3 CalculatePressureTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			uint32_t memberCount, const Vector3& targetPos) const;
		Vector3 CalculateDistributedTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			uint32_t memberCount, const Vector3& targetPos, const Vector3& forward) const;
		Vector3 CalculateSpiralTarget(const CrowdBehaviorSettings& behavior, uint32_t memberIndex,
			uint32_t memberCount, const Vector3& targetPos) const;
		Vector3 CalculateHuntingTarget(uint32_t memberIndex, const Vector3& targetPos,
			const Vector3& forward, const Vector3& right) const;
		Vector3 CalculateRingPosition(const Vector3& center, float radius, float angle) const;

	private:
		float elapsedTime_ = 0.0f;	// 動き続けるパターンで共通利用する経過時間
	};
}
