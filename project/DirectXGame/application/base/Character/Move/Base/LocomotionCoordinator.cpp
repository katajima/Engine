#include "LocomotionCoordinator.h"


MoveCommand LocomotionCoordinator::BuildCommand()
{
	MoveCommand cmd{};
	cmd.finalVelocity = {0,0,0};
	if (!ctx) return cmd;
	int pro = 999;
	for (auto& request : requests_) {
		cmd.finalVelocity += request.velocity;
		
		if (pro >= request.priority) {
			pro = request.priority;
			cmd.finalDirection = request.direction;
		}

		if (request.isLanding) {
			cmd.isLanding = true;
			cmd.groundHeight = request.groundHeight;
		}

	}


	return cmd;
}
