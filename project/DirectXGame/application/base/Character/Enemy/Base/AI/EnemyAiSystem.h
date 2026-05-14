#pragma once
#include "EnemyAttackRequestSystem.h"
#include "EnemyAttackSlotSystem.h"
#include <memory>

namespace Character {

	class BaseEnemy;

	class EnemyAiSystem {
	public:

		// 初期化
		void Initialize();

		void UpdateSlot(
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos,
			float targetRotateY
		);

		void UpdateRequest(const std::vector<BaseEnemy*>& enemies, float dt);

		EnemyAttackSlotSystem* GetAttackSlotSystem() const { return attackSlotSystem_.get(); }

	private:
		std::unique_ptr<EnemyAttackRequestSystem> attackRequestSystem_ = nullptr;
		std::unique_ptr<EnemyAttackSlotSystem> attackSlotSystem_ = nullptr;
	};
}