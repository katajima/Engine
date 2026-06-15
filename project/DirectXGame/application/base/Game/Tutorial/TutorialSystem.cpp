#include "TutorialSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

void TutorialSystem::Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager, 
    Engine::GlobalVariables* globalVariables, Character::BaseCharacter* character) {
    this->sceneManager = sceneManager;
    this->input = input;
    this->entityManager = entityManager;
    this->globalVariables = globalVariables;
    this->character = character;


}

void TutorialSystem::Update(float dt){
    if (currentStep_ >= steps_.size()) {
        return;
    }

    steps_[currentStep_]->Update(dt, character);

    if (steps_[currentStep_]->IsCompleted()) {

        steps_[currentStep_]->Exit();

        currentStep_++;

        if (currentStep_ < steps_.size()) {
            steps_[currentStep_]->Enter();
        }
    }
}
