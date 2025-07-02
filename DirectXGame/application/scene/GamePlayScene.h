#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/base/Timer.h"

// application
#include"DirectXGame/application/base/BaseClass/Character/BaseCharacterManeger.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Stage/Stage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/BaseClass/Bullet/BulletManager.h"

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
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// 宇宙カメラ
	std::unique_ptr<UniverseCamera> universeCamera_;


	bool flag = true;
	bool isUniverseCamera = false;
	float timer = 0.0f;
	float cameraScaleT = 0.0f;
	float minScaleZCamera = 5.5f;
private:
	// ライト
	std::shared_ptr<DirectionalLight> directional;
private:
	// キャラクター管理
	std::unique_ptr<BaseCharacterManager> caracterManager_;
	// 敵ポジション
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
	// デバッグ用タイマー
	DebugTimer debugTimer_;
	DebugTimer debugTimerAll_;
private:
	// ゲームUI
	std::unique_ptr<GameUI> gameUI = std::make_unique<GameUI>();
};

