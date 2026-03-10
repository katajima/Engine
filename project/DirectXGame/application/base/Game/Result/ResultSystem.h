#pragma once
// C++
#include <imgui.h>
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

// 前方宣言
namespace Engine {
	class DirectXCommon;
	class EntityManager;
	class GlobalVariables;
	class SceneManager;
}
class EffectSystem;
class InputSystem;

class ResultSystem {
public:

	void fhfhfh();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="entity3DManager"></param>
	/// <param name="entity2DManager"></param>
	/// <param name="globalVariables"></param>
	void Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	/// <summary>
	/// 選択されているインデックスの取得
	/// </summary>
	/// <returns></returns>
	int GetSelectedIndex() const { return selectedIndex; }


private:
	// 選択されているインデックス
	int selectedIndex = 0;
	float timer_ = 0.0f;
	float changeTimer_ = 0.0f;
	float changeCooldown_ = 0.5f;
private:
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::SceneManager* sceneManager = nullptr;
	InputSystem* input = nullptr;
};