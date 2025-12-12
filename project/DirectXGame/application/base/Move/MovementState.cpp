#include "MovementState.h"
#include "MoveSystem.h"
#include "JumpSystem.h"

void GroundState::Update(float dt, Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, Engine::Input* input, MoveSystem* moveSystem, JumpSystem* jumpSystem) {

}

void AirState::Update(float dt, Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, Engine::Input* input, MoveSystem* moveSystem, JumpSystem* jumpSystem) {

}

void MovementStateMachine::Update(float dt, Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, Engine::Input* input, MoveSystem* moveSystem, JumpSystem* jumpSystem) {

};
