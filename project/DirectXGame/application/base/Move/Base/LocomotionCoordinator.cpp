#include "LocomotionCoordinator.h"


MoveCommand LocomotionCoordinator::BuildCommand()
{
	MoveCommand cmd{};

	if (!ctx) return cmd;
	int pro = 999;
	for (auto& request : requests_) {
		cmd.finalVelocity += request.velocity;
		
		if (pro >= request.priority) {
			pro = request.priority;
			cmd.finalDirection = request.direction;
		}
	}


	return cmd;
}
