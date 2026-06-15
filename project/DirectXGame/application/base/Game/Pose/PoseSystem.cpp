#include "PoseSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

// 初期化
void PoseSystem::Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
    this->sceneManager = sceneManager;
    this->input = input;
    this->entityManager = entityManager;
    this->globalVariables = globalVariables;

}

// 更新
void PoseSystem::Update(float dt) {

}
