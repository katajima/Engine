#pragma once
#include <cstdint>

namespace Character {

	enum class EnemyAttackRing {
		None,
		TooClose,
		Attack,
		Standby,
		Approach,
	};

	class EnemyAttackRequest {
	public:
		void Activate() {
			isActive = true;
		}

		void Deactivate() {
			isActive = false;
			isGranted = false;
			number = 0;
			ring = EnemyAttackRing::None;
		}

		void Grant() {
			isGranted = true;
		}

		void Deny() {
			isGranted = false;
		}

		void SetDistance(float value) {
			distance = value;
		}

		float GetDistance() const {
			return distance;
		}

		void SetRing(EnemyAttackRing value) {
			ring = value;
		}

		EnemyAttackRing GetRing() const {
			return ring;
		}

		void StartCooldown(float time) {
			cooldownTimer = time;
		}

		void UpdateCooldown(float dt) {
			if (cooldownTimer > 0.0f) {
				cooldownTimer -= dt;
				if (cooldownTimer < 0.0f) {
					cooldownTimer = 0.0f;
				}
			}
		}

		bool IsCooldown() const {
			return cooldownTimer > 0.0f;
		}

		bool GetIsActive() const { return isActive; }
		bool GetIsGranted() const { return isGranted; }

		uint32_t GetNumber() const { return number; }
		void SetNumber(uint32_t value) { number = value; }

	private:
		float distance = 0.0f;
		float cooldownTimer = 0.0f;

		EnemyAttackRing ring = EnemyAttackRing::None;

		uint32_t number = 0;

		bool isActive = false;
		bool isGranted = false;
	};
}
