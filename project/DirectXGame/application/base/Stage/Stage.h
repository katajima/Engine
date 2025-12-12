#pragma once

// C++
#include <imgui.h>
#include <list>


// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"

#include<DirectXGame/engine/Effect/EffectComponent.h>

#include "DirectXGame/application/base/Light/BaseLights.h"

// 前方宣言
namespace Engine{
	class DirectXCommon;
	class Entity3DManager;
	class Entity2DManager;
}
/// <summary>
/// ステージクラス
/// </summary>
class Stage {
public:
	// 初期化
	void Initialize(Engine::DirectXCommon* dxcommon, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::Camera* camera);

	// 更新
	void Update();

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
private:
	/// <summary>
	/// エミッター初期化
	/// </summary>
	void InitEmit();

	/// <summary>
	/// エミッター更新
	/// </summary>
	void EmitUpdate();

public:
	std::vector<Engine::Object3d*> missiles_;
private:
	// 空
	Engine::Object3d* sky_;

	
	// 地面
	Engine::Object3d* tail_;
	

	// 列車
	Engine::Object3d* train_;
	float trainSpeed_ = 300.0f;
	float trainStartX_ = -3110.0f;
	float trainEndX_ = 4000.0f;
	float trainWarpTime_ = 0.0f;
	float trainWarpTimeMax_ = 20.0f;

	// 船
	Engine::Object3d* ship_;
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };

	// 円運動のパラメータ
	float radius = 1000.0f; // 円の半径
	float angularSpeed = DirectX::XM_PI * 0.5f * 0.25f; // ラジアン/秒（90度/秒）
	
	Vector3 center = { -3111,-50,3040 }; // 中心座標を指定


	// オーシャンシェーダー
	std::unique_ptr<Engine::Ocean> ocean_ = nullptr;
	Engine::Object3d* oceanObject;


	// スカイボックス
	std::unique_ptr<Engine::SkyBox> skyBox;
	

	// ライト
	std::shared_ptr<Engine::DirectionalLight> directional;
private:

private:
	Engine::DirectXCommon* dxCommon_;
	Engine::Entity3DManager* entity3DManager_;
	Engine::Entity2DManager* entity2DManager_;

	Engine::Camera* camera_ = nullptr;
private: // 一旦

	struct ProvisionalData {
		Vector3 oceanTranslate = { 0, -30, 0 };
		Vector3 oceanRotate = { Math::DegreesToRadians(90) ,0,0 };

		Vector3 skyBoxScale = {100,100,100};


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
