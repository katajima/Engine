#include "ResponseMoveSystem.h"

void ResponseMoveSystem::Initialize(){

}

void ResponseMoveSystem::Update(const LocomotionContext& ctx, LocomotionCoordinator& coordinator) {
	if (ctx.state == Character::CharacterMainState::Attack) {
		coordinator.Request(reuest);
	}
	reuest.velocity = 0.0f;
	reuest.priority = 999;
}
