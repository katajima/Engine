#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/collider/2d/CollisionManager2D.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"

#include"DirectXGame/engine/base/Load/LoadLevelData.h"
#include<DirectXGame/engine/Effect/Particle/2d/ParticleEmitter2d.h>

// application
#include"DirectXGame/engine/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"

#include"DirectXGame/engine/Camera/CameraManager.h"
#include"DirectXGame/engine/3d/Object/Object3dInstanceManager.h"

#include "DirectXGame/engine/Audio/Audio.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"
#include <DirectXGame/engine/Effect/EffectComponent.h>

#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Animation/AnimationComponent.h"
/// <summary>
/// サンプルシーン
/// </summary>
class SampleScene : public Engine::BaseScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画3d
	/// </summary>
	void Draw3D() override;
	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D() override;


	/// <summary>
	/// ImGui更新
	/// </summary>
	void UpdateImGui();

	/// <summary>
	/// グローバルバリアブル適応
	/// </summary>
	void ApplyGlobalVariables();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

private:
	/// <summary>
	/// カメラ初期化
	/// </summary>
	void InitCamera();
	/// <summary>
	/// ライト初期化
	/// </summary>
	void InitLight();
	/// <summary>
	/// パーティクル初期化
	/// </summary>
	void InitParticle();
	/// <summary>
	/// オブジェクト初期化
	/// </summary>
	void InitObject();
	/// <summary>
	/// スプライト初期化
	/// </summary>
	void InitSprite();


private:
	Engine::Input* input_ = nullptr;									// 入力
	// シーン内の入力をまとめて管理する
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;

	std::unique_ptr<Engine::Audio> audioTest_;							// オーディオ
	uint32_t soundHandle_ = 0;									// サウンドハンドル
private:
	Engine::Object3d* testObject_ = nullptr;							// テスト用オブジェクト

	std::unique_ptr<Engine::SkyBox> skyBox;								// スカイボックス
	Engine::Object3d* sky_;												// スカイボックス用オブジェクト

	Engine::Object3dInstanceManager* object3dInstanceManager_ = nullptr;	// 3Dオブジェクトインスタンスマネージャ
private:
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_;
private:


	// 衝突マネージャ
	std::unique_ptr<Engine::CollisionManager> collisionManager_;
	std::unique_ptr<Engine::CollisionManager2d> collisionManager2d_;

	// ライト
	std::shared_ptr<Engine::DirectionalLight> directional;
private:
	// パーティクルエミッター
	std::unique_ptr<Engine::AABBParticleEmitter> aabbParticleEmitter_ = nullptr;
	// パーティクルエミッター
	std::unique_ptr<Engine::SphereParticleEmitter> sphereParticleEmitter = nullptr;
	// パーティクルエミッター
	std::unique_ptr<Engine::CornerParticleEmitter> cornerParticleEmitter = nullptr;
	// パーティクルエミッター
	std::unique_ptr<Engine::SplineParticleEmitter> splineParticleEmitter = nullptr;
	// パーティクルエミッター
	std::unique_ptr<Engine::LineParticleEmitter> lineParticleEmitter = nullptr;
	// パーティクルエミッター
	std::unique_ptr<Engine::TriangleParticleEmitter> triangleParticleEmitter = nullptr;
	// パーティクルエミッター
	std::unique_ptr<Engine::MeshParticleEmitter> meshParticleEmitter = nullptr;


	std::unique_ptr<Engine::ParticleEmitter2d> particleEmitter2d_ = nullptr;

private:
	// スプライト
	std::unique_ptr<Engine::Sprite> sprite_ = nullptr;

	std::unique_ptr<Engine::PlanePrimitive> plane_;

	// スプライト
	std::unique_ptr<Engine::Sprite> sprite2dP_ = nullptr;

};


