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
#include"DirectXGame/application/base/Camera/FixedCamera/StageCamera.h"

#include"DirectXGame/application/base/Stage/MainStage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include"DirectXGame/application/base/Effect/Effect.h"


#include "DirectXGame/application/base/Input/InputCoordinator.h"
#include <DirectXGame/application/base/Game/GameFlowController.h>

#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include "DirectXGame/application/base/Stage/StageColliderSystem.h"

#include <DirectXGame/application/base/Game/Pose/PoseSystem.h>
#include <DirectXGame/application/base/UI/PoseUI.h>

#include <DirectXGame/application/base/Object/CollisionRegistrationSystem.h>
#include "DirectXGame/application/base/Game/GameplaySession.h"

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
	void ApplyGlobalVariables() {};
private:
	Engine::Input* input = nullptr;
	Engine::Audio* audio = nullptr;
	DebugTimer debugTimer_;
	// ゲームプレイシーンで共通利用する基盤システム
	std::unique_ptr<GameplaySession> gameplaySession_ = nullptr;

	// ゲームの流れやルールの管理をするクラス
	std::unique_ptr<GameFlowController> gameFlowController_ = nullptr;

	// ポーズシステム
	std::unique_ptr<PoseSystem> poseSystem_;
	// ポーズUI
	std::unique_ptr<PoseUI> poseUI_;
private:
	// 宇宙カメラ
	std::unique_ptr<UniverseCamera> universeCamera_ = nullptr;
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_ = nullptr;
	// ステージカメラ
	std::unique_ptr<StageCamera> stageCamera_ = nullptr;

	// ステージ
	std::unique_ptr<MainStage> stage_ = nullptr;
private:
	// ゲームUI
	std::unique_ptr<GameUI> gameUI = nullptr;
};

