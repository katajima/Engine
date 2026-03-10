#include "AttackMoveSystem.h"


void AttackMoveSystem::Initialize() {

}

void AttackMoveSystem::Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {
	if (ctx.state == Character::CharacterMainState::Attack) {
		coordinator.Request(reuest);
	}
	reuest.velocity = 0.0f;
	reuest.priority = 999;
};
