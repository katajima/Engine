#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/collider/2d/CollisionManager2D.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"

#include"DirectXGame/engine/base/Load/LoadLevelData.h"
#include<DirectXGame/engine/Effect/Particle/2d/ParticleEmitter2d.h>

// application
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"

#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include"DirectXGame/engine/3d/Object/Object3dInstansManager.h"

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
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画3d
	void Draw3D() override;
	// 描画2d
	void Draw2D() override;


	// ImGui更新
	void UpdateImGui();

	// グローバルバリアブル適応
	void ApplyGlobalVariables();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

private:
	// カメラ初期化
	void InitCamera();
	// ライト初期化
	void InitLight();
	// パーティクル初期化
	void InitParticle();
	// オブジェクト初期化
	void InitObject();
	// スプライト初期化
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

	Engine::Object3dInstansManager* object3dInstansManager_ = nullptr;	// 3Dオブジェクトインスタンスマネージャ
private:
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManeger_;
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


