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
	class EntityManager;
	class GlobalVariables;
}
class Effect;


/// <summary>
/// タイトルシーンステージ
/// </summary>
class TitleStage {
public:
	// 初期化
	void Initialize(Engine::DirectXCommon* dxcommon, Engine::EntityManager* entityManager,
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
	/// <summary>
	/// エミッター初期化
	/// </summary>
	void InitEmit();

	/// <summary>
	/// エミッター更新
	/// </summary>
	void EmitUpdate();

	//
	void InitializeStone();
private:
	Engine::DirectXCommon* dxCommon = nullptr;
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::Camera* camera = nullptr;
	// エフェクト
	Effect* effect = nullptr;
private:
	// 空
	Engine::Object3d* sky_;
	// スカイボックス
	std::unique_ptr<Engine::SkyBox> skyBox;
	// ライト
	std::shared_ptr<Engine::DirectionalLight> directional;
	std::shared_ptr<Engine::PointLight> pointLight;
	// タイル
	Engine::Object3d* tail;


	// 石
	Engine::Object3d* stone1_;
	Engine::Object3d* stone2_;
	Engine::Object3d* stone3_;
	Engine::Object3d* stone4_;
	Engine::Object3d* stone5_;

	
	// プレイヤー車
	std::unique_ptr<PlayerCar> playerCar_;
	// 位置
	Vector3 playerCarPos_ = { -1.25f,0.05f,12.0f };

private:
	struct ProvisionalData {
		Vector3 oceanTranslate = { 0, -30, 0 };
		Vector3 oceanRotate = { Math::DegreesToRadians(90) ,0,0 };

		Vector3 skyBoxScale = { 1000,1000,1000 };


		Vector2 oceanRange{ 8000.0f,8000.0f };
		float oceanAmplitude = 16.5f;
		Vector2 waveDirection = { 0.5f,0.5f };
		float waveSpeed = 5.0f;
		Color color = { 0.0f, 0.0f, 0.8f, 0.75f };

		int missileNumX = 5;
		int missileNumY = 2;

		Vector3 missileTranslate = { 3500,106,3000 };
		Vector2 missileInterval = { 120.0f,100.0f };
		Vector3 missileRotate = { 0 ,Math::DegreesToRadians(-90),0 };
		float  missileSize = 10.0f;
	};
	ProvisionalData provisionalData_;
};