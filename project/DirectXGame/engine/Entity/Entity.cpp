#include "Entity.h"

void Engine::Entity::Update(float deltaTime) { mDeltaTime = deltaTime; Update(); }

void Engine::Entity::RigidBodyUpdate(float deltaTime) { mDeltaTime = deltaTime; RigidBodyUpdate(); }

void Engine::Entity::SetDeltaTime(float deltaTime) { mDeltaTime = deltaTime; }

float Engine::Entity::GetDeltaTime() const { return mDeltaTime; }

