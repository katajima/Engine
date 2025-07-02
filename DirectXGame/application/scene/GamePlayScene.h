#pragma once
#include"DirectXGame/engine/2d/SpriteCommon.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/audio/Audio.h"
#include"DirectXGame/engine/3d/Model/ModelManager.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/3d/Model/ModelCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Light/LightCommon.h"


// application
#include"DirectXGame/application/base/Character/BaseCharacterManeger.h"
//#include"DirectXGame/application/base/Enemy/Base/EnemyManager.h"
//#include"DirectXGame/application/base/Player/Player.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Stage/Stage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/Bullet/BulletManager.h"


#include "DirectXGame/engine/base/Timer.h"
// ゲームプレイシーン
class GamePlayScene : public BaseScene
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



	void UpdateImGui();

	
	void ApplyGlobalVariables();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

private:

	void InitializeResources();
	void InitializeCamera();

private:
	// 振るまい
	enum class Behavior {
		kPhase1,	// フェーズ１
		kPhase2,	// フェーズ２
	};

	//振るまい
	Behavior behavior_ = Behavior::kPhase1;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	
	//
	void BehaviorPhase1Initialize();
	//
	void BehaviorPhase1Update();
	//
	void BehaviorPhase2Initialize();
	//
	void BehaviorPhase2Update();
	
private:
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
private:
	// カメラ
	std::unique_ptr < Camera> camera;

	
	bool flag = true;
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	// 宇宙カメラ
	std::unique_ptr<UniverseCamera> universeCamera_;

	bool isUniverseCamera = false;
	float timer = 0.0f;
	float cameraScaleT = 0.0f;

	float minScaleZCamera = 5.5f;

private:
	// ライト
	std::shared_ptr<DirectionalLight> directional;
private:
	

	std::unique_ptr<BaseCharacterManager> caracterManager_;

	// プレイヤー
	//std::unique_ptr<Player> player_;

	
	Vector3 enemyPosition = Vector3(0, 0, 0);


	// ステージ
	std::unique_ptr<Stage> stage_;

	// 弾
	std::unique_ptr<BulletManager> bulletManager_;

private:

	
	// 衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;

	// シーン遷移用
	int count = 0;
	int sceneCount = 0;
	int clock = 1;
	
	DebugTimer debugTimer_;
	DebugTimer debugTimerAll_;

private:
	
	// ゲームUI
	std::unique_ptr<GameUI> gameUI = std::make_unique<GameUI>();

};

