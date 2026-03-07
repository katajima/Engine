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
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Game/Select/SelectSystem.h"

// 前方宣言
namespace Engine {
	class DirectXCommon;
	class EntityManager;
}

/// <summary>
/// リザルト用ステージクラス
/// </summary>
class ResultStage {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="entity3DManager"></param>
	/// <param name="entity2DManager"></param>
	/// <param name="globalVariables"></param>
	/// <param name="camera"></param>
	void Initialize(Engine::EntityManager* entity3DManager,
		Engine::GlobalVariables* globalVariables, Engine::Camera* camera);


	// 更新
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
	/// 描画2d
	/// </summary>
	void Draw2D();

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


	// 換金所
	Engine::Object3d* moneyExchangePlace;
private:
	// 位置
	Vector3 playerCarPos_ = { -1.25f,0.05f,-4.0f };

private:
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::Camera* camera = nullptr;
	// エフェクト
	Effect* effect = nullptr;
};