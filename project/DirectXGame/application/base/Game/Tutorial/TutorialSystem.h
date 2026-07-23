#pragma once
#include <list>
// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include<DirectXGame/engine/Effect/EffectComponent.h>
#include "DirectXGame/application/base/Light/BaseLights.h"

#include "TutorialStep.h"

// 前方宣言
namespace Engine {
    class DirectXCommon;
    class EntityManager;
    class GlobalVariables;
    class SceneManager;
}

namespace Character {
	class BaseCharacter;
}

class EffectSystem;
class InputSystem;


/// <summary>
/// TutorialSystemを管理・実装するクラス。
/// </summary>
class TutorialSystem {
public:
    TutorialSystem() {}
    ~TutorialSystem() {}

	// 初期化
    void Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
        Engine::GlobalVariables* globalVariables, Character::BaseCharacter* character);
    
    // 更新
    void Update(float dt);

	// イベント送信
    void SendEvent(const TutorialEvent& event);


private:
    std::vector<std::unique_ptr<TutorialStep>> steps_;

    uint32_t currentStep_ = 0;
private:
    Engine::EntityManager* entityManager = nullptr;
    Engine::GlobalVariables* globalVariables = nullptr;
    Engine::SceneManager* sceneManager = nullptr;
    Character::BaseCharacter* character = nullptr;
    InputSystem* input = nullptr;
};
