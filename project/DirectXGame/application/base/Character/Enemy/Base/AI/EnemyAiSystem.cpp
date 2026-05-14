#include "EnemyAiSystem.h"

namespace Character {

	void EnemyAiSystem::Initialize() {
		attackRequestSystem_ = std::make_unique<EnemyAttackRequestSystem>();
		attackRequestSystem_->Initialize();

		attackSlotSystem_ = std::make_unique<EnemyAttackSlotSystem>();
		attackSlotSystem_->Initialize();
	}

	void EnemyAiSystem::UpdateSlot(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float targetRotateY
	) {
		attackSlotSystem_->Update(enemies, targetPos, targetRotateY);
	}

	void EnemyAiSystem::UpdateRequest(const std::vector<BaseEnemy*>& enemies, float dt) {
		attackRequestSystem_->Update(enemies, dt);
	}
}
