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


#include "DirectXGame/application/base/Car/PlayerCar.h"

// 前方宣言
namespace Engine {
	class DirectXCommon;
	class Entity3DManager;
	class Entity2DManager;
	class GlobalVariables;
}
class Effect;

/// <summary>
/// セレクト用ステージ
/// </summary>
class SelectStage {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="entity3DManager"></param>
	/// <param name="entity2DManager"></param>
	/// <param name="globalVariables"></param>
	/// <param name="camera"></param>
	void Initialize(Engine::Entity3DManager* entity3DManager,Engine::Entity2DManager* entity2DManager, 
		Engine::GlobalVariables* globalVariables, Engine::Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	///< summary>
	/// 描画
	///</summary>
	void Draw();

	/// <summary>
	/// 描画エフェクト
	/// </summary>
	void DrawP();


	/// <summary>
	/// エフェクト
	/// </summary>
	/// <param name="effect"></param>
	void SetEffect(Effect* effect) {
		this->effect = effect;
		playerCar_->SetEffect(effect);
	}
	/// <summary>
	/// 車
	/// </summary>
	/// <returns></returns>
	PlayerCar* GetPlayerCar() { return playerCar_.get(); }

	void SetRatio(float ratio) { ratio_ = ratio; }

private:
	Engine::DirectXCommon* dxCommon = nullptr;
	Engine::Entity3DManager* entity3DManager = nullptr;
	Engine::Entity2DManager* entity2DManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::Camera* camera = nullptr;
	// エフェクト
	Effect* effect = nullptr;
private:
	// プレイヤー車
	std::unique_ptr<PlayerCar> playerCar_;
	// 空
	Engine::Object3d* sky_;
	// スカイボックス
	std::unique_ptr<Engine::SkyBox> skyBox;
	// ライト
	std::shared_ptr<Engine::DirectionalLight> directional;
	// タイル
	Engine::Object3d* tail;


	std::vector<std::unique_ptr<ObjectComponent>> stoneComponents_;
private:
	// 位置
	Vector3 playerCarPos_ = { -1.25f,0.05f,12.0f };
	//
	float ratio_ = 1.0f;
};