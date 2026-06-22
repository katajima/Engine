#include "SelectSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

void SelectSystem::Initialize(Engine::SceneManager* sceneManager ,InputSystem* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->sceneManager = sceneManager;
	this->input = input;
}

void SelectSystem::Update(float dt) {

	timer_ += dt;

	if (timer_ >= 0.25f) {
		if (input->GetGameInputData().moveShick.y > 0.5f) {
			selectedIndex += -1;
			timer_ = 0.0f;
		}
		else if (input->GetGameInputData().moveShick.y < -0.5f) {
			selectedIndex += 1;
			timer_ = 0.0f;
		}
	}

	if (selectedIndex < 0) {
		selectedIndex = 0;
	}
	else if (selectedIndex > 3) {
		selectedIndex = 3;
	}


	if (input->GetGameInputData().decisionTrigger) {
		if (selectedIndex == 0) {
			sceneManager->ChangeScene("GAMEPLAY");
		}
		else if (selectedIndex == 1) {
			//sceneManager->ChangeScene("TUTORIAL");
		}
		else if (selectedIndex == 2) {
			//sceneManager->ChangeScene("CUSTOM");
		}
		else if (selectedIndex == 3) {
			sceneManager->ChangeScene("TITLE");
		}
	}
}
