#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/base/Character/Base/BaseCharacterManeger.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"

#include"DirectXGame/application/base/Stage/Stage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include"DirectXGame/application/base/Effect/Effect.h"


#include "DirectXGame/application/base/Input/InputHander.h"
#include <DirectXGame/application/base/Game/GameFlowController.h>

/// <summary>
/// ゲームプレイシーン
/// </summary>
class GamePlayScene : public BaseScene
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
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// インプットハンドラ
	std::unique_ptr < InputHander> inputHander_;
	ICommand* iCommand_;

	static float nowTime;
	float fps = 0.0f;

	// ゲームの流れやルールの管理をするクラス
	std::unique_ptr<GameFlowController> gameFlowController_ = nullptr;
private:
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
	std::unique_ptr<UICount> sprite;
private:
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// 宇宙カメラ
	std::unique_ptr<UniverseCamera> universeCamera_;
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManeger_;
	
	std::unique_ptr<EffectComponent> effectComponent_ = nullptr;

	// 
	std::unique_ptr<Effect> effect_;
private:
	// キャラクター管理
	std::unique_ptr<BaseCharacterManager> caracterManager_;
	// 敵ポジション
	Vector3 enemyPosition = Vector3(0, 0, 0);
	// ステージ
	std::unique_ptr<Stage> stage_;
	// 弾
	std::unique_ptr<BulletManager> bulletManager_;
	// レベルデータ
	std::unique_ptr<LoadLevelData> loadData_;
private:
	// 衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;
	// シーン遷移用
	int count = 0;
	int sceneCount = 0;
	int clock = 1;

	float tumeee_ = 0.0f;
private:
	// ゲームUI
	std::unique_ptr<GameUI> gameUI;
};

