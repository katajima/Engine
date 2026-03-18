#include "ResponseMoveSystem.h"

void ResponseMoveSystem::Initialize() {

}

void ResponseMoveSystem::Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {
	
	for (auto& re : requests) {
		coordinator.Request(re);
	}
	requests.clear();
}
