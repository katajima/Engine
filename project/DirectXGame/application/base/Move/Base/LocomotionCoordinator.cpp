#include "LocomotionCoordinator.h"


MoveCommand LocomotionCoordinator::BuildCommand()
{
	MoveCommand cmd{};

	if (!ctx_) return cmd;

	for (auto& request : requests_) {
		cmd.finalVelocity += request.velocity;
	}


	return cmd;
}
