#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
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
#include <DirectXGame/application/base/Input/InputManager.h>
#include "DirectXGame/application/base/Input/InputSystem.h"
#include <DirectXGame/application/base/Game/GameFlowController.h>

#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>

/// <summary>
/// ゲームプレイシーン
/// </summary>
class GamePlayScene : public Engine::BaseScene
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

private:
	// ImGui更新
	void UpdateImGui();

	// グローバルバリアブル適応
	void ApplyGlobalVariables();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

private:
	Engine::Input* input_ = nullptr;
	Engine::Audio* audio_ = nullptr;

	// インプットハンドラ
	std::unique_ptr < Character::InputHander> inputHander_;
	Character::ICommand* iCommand_;

	// インプットマネージャ
	std::unique_ptr<InputManager> inputManager_;

	// インプットシステム
	std::unique_ptr<InputSystem> inputSystem_;

	// ゲームの流れやルールの管理をするクラス
	std::unique_ptr<GameFlowController> gameFlowController_ = nullptr;
private:
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// 宇宙カメラ
	std::unique_ptr<UniverseCamera> universeCamera_;
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_;
	
	std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;

	// 
	std::unique_ptr<Effect> effect_;

	// スペシャルポイント管理クラス
	std::unique_ptr<SpecalPointManager> specalPointManager_ = nullptr;

private:
	// キャラクター管理
	std::unique_ptr<Character::CharacterManager> characterManager_;
	// ステージ
	std::unique_ptr<MainStage> stage_;
	// 弾
	std::unique_ptr<BulletManager> bulletManager_;
	// レベルデータ
	std::unique_ptr<LoadLevelData> loadData_;
private:
	// 衝突マネージャ
	std::unique_ptr<Engine::CollisionManager> collisionManager_;
private:
	// ゲームUI
	std::unique_ptr<GameUI> gameUI;
};

