#include "CustomSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

void CustomSystem::Initialize(Engine::SceneManager* sceneManager, InputSystem* input,
	Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables){
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->sceneManager = sceneManager;
	this->input = input;
}

void CustomSystem::Update(float dt){

}
