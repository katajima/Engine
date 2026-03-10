#include "ResponseMoveSystem.h"

void ResponseMoveSystem::Initialize() {

}

void ResponseMoveSystem::Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {
	coordinator.Request(reuest);
	reuest.velocity = 0.0f;
	reuest.priority = 999;
}
