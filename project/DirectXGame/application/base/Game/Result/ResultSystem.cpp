#include "ResultSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

void ResultSystem::Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->sceneManager = sceneManager;
	this->input = input;
}

void ResultSystem::Update(float dt) {

	timer_ += dt;
	changeTimer_ += dt;

	if (timer_ >= 0.25f) {
		if (input->GetGameInputData().moveShick.x > 0.5f) {
			selectedIndex += 1;
			timer_ = 0.0f;
		}
		else if (input->GetGameInputData().moveShick.x < -0.5f) {
			selectedIndex += -1;
			timer_ = 0.0f;
		}
	}

	if (selectedIndex < 0) {
		selectedIndex = 0;
	}
	else if (selectedIndex > 1) {
		selectedIndex = 1;
	}
	if (changeTimer_ >= changeCooldown_) {
		changeTimer_ = changeCooldown_;
		if (input->GetGameInputData().decisionTrigger) {
			if (selectedIndex == 0) {
				sceneManager->ChangeScene("GAMEPLAY");
			}
			else if (selectedIndex == 1) {
				sceneManager->ChangeScene("TITLE");
			}
		}
	}
}