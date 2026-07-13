#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/StageCamera.h"

#include"DirectXGame/application/base/Stage/MainStage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include"DirectXGame/application/base/Camera/Base/CameraManager.h"
#include"DirectXGame/application/base/Effect/Effect.h"


#include "DirectXGame/application/base/Input/InputCoordinator.h"

#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include "DirectXGame/application/base/Stage/StageColliderSystem.h"

#include <DirectXGame/application/base/Game/Tutorial/TutorialSystem.h>
#include <DirectXGame/application/base/Stage/TutorialStage.h>
#include <DirectXGame/application/base/UI/TutorialUI.h>

#include <DirectXGame/application/base/Game/Pose/PoseSystem.h>
#include <DirectXGame/application/base/UI/PoseUI.h>

#include <DirectXGame/application/base/Object/CollisionRegistrationSystem.h>
#include "DirectXGame/application/base/Game/GameplaySession.h"

class TutorialScene : public Engine::BaseScene {
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
	void UpdateImGui() {};

	// グローバルバリアブル適応
	void ApplyGlobalVariables() {};
private:
	Engine::Input* input = nullptr;
	// ゲームプレイシーンで共通利用する基盤システム
	std::unique_ptr<GameplaySession> gameplaySession_ = nullptr;
private:
	// チュートリアルステージ
	std::unique_ptr<TutorialStage> tutorialStage_;
	// チュートリアルシステム
	std::unique_ptr<TutorialSystem> tutorialSystem_;
	// チュートリアルUI
	std::unique_ptr<TutorialUI> tutorialUI_;
	// ポーズシステム
	std::unique_ptr<PoseSystem> poseSystem_;
	// ポーズUI
	std::unique_ptr<PoseUI> poseUI_;

};
