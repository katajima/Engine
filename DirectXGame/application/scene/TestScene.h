#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include "DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Camera/DebugCamera.h"
#include"DirectXGame/engine/3d/Object3d.h"
#include "DirectXGame/engine/base/TextureManager.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/audio/Audio.h"
#include"DirectXGame/engine/3d/Object3dCommon.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/2d/SpriteCommon.h"
#include"DirectXGame/engine/2d/Sprite.h"

#include"DirectXGame/engine/base/ImGuiManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleEmitter.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include"DirectXGame/engine/effect/Trail/TrailEffect.h"

#include"DirectXGame/engine/Primitive/Primitive.h"

class TestScene : public BaseScene
{
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw3D() override;

	void Draw2D() override;

private: // 各初期化

	void InitializeObject3D();

	void InitializeObject2D();

	void InitializeParticle();

	void InitializeLight();

	void InitializeCamera();
private:
	// 振るまい(シーンの部屋)
	enum class SceneBehavior {
		kSceneRoom01,   // 海、スカイボックス、スプライト
		kSceneRoom02,   // パーティクル
		kSceneRoom03,   // マルチマテリアルやマルチメッシュ
		kSceneRoom04,   // スキニング用
		kSceneRoom05,   
		kSceneRoom06,   
		kSceneRoom07,   
		kSceneRoom08,   
		kSceneRoom09,   
		kSceneRoom10,   
	};

	//振るまい
	SceneBehavior behavior_ = SceneBehavior::kSceneRoom01;
	// 次の振るまいリクエスト
	std::optional<SceneBehavior> behaviorRequest_ = std::nullopt;

private:

	// 各ルーム初期化
	void InitializeRoom01();
	void InitializeRoom02();
	void InitializeRoom03();
	void InitializeRoom04();
	void InitializeRoom05();
	void InitializeRoom06();
	void InitializeRoom07();
	void InitializeRoom08();
	void InitializeRoom09();
	void InitializeRoom10();

	// 更新
	void UpdateRoom01();
	void UpdateRoom02();
	void UpdateRoom03();
	void UpdateRoom04();
	void UpdateRoom05();

	// 部屋切り替え
	void SwitchRoom();

private:
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ImGuiManager* imGuiManager = nullptr;


	/// <summary>
	/// カメラ
	/// </summary>
	
	std::unique_ptr < Camera> camera;
	std::unique_ptr < DebugCamera> debugCamera;
	
	bool isDebugCamera = false;

	/// <summary>
	/// 3Dオブジェクト
	/// </summary>

	Object3d skinningObject;
	Object3d skinningObject2;
	Object3d tail;
	Object3d multiy;

	// オーシャンシェーダー
	std::unique_ptr < Ocean> ocean_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	
	std::vector<std::unique_ptr<Sprite>> sprite_;


	/// <summary>
	/// パーティクルエミッタ
	/// </summary>

	std::unique_ptr<ParticleEmitter> emitter_  =nullptr;

	std::unique_ptr<ParticleEmitter> emitterEnemy_  =nullptr;

	/// <summary>
	/// ライト
	/// </summary>

	std::shared_ptr<PointLight> point;

	std::shared_ptr<SpotLight> spot;

	std::shared_ptr<DirectionalLight> directional;

};



