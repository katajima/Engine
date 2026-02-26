#include "AttackMoveSystem.h"


void AttackMoveSystem::Initialize() {

}

void AttackMoveSystem::Update(const LocomotionContext& ctx, LocomotionCoordinator& coordinator, Engine::WorldTransform& world, InputSystem* input) {
	if (ctx.state == Character::CharacterMainState::Attack) {
		coordinator.Request(reuest);
	}
	reuest.velocity = 0.0f;
};
