#include "MovementState.h"
#include "MoveSystem.h"
#include "JumpSystem.h"

void GroundState::Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input, MoveSystem* moveSystem, JumpSystem* jumpSystem) {

}

void AirState::Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input, MoveSystem* moveSystem, JumpSystem* jumpSystem) {

}

void MovementStateMachine::Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input, MoveSystem* moveSystem, JumpSystem* jumpSystem) {

};
