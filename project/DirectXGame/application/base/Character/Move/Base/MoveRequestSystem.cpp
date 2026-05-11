#include "MoveRequestSystem.h"


void MoveRequestSystem::Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {

	for (auto& request : requests) {
		coordinator.Request(request);
	}
	requests.clear();

};
