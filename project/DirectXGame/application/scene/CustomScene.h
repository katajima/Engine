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

#include"DirectXGame/application/base/Stage/MainStage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"

#include <DirectXGame/application/base/UI/CustomUI.h>
#include <DirectXGame/application/base/Game/Custom/CustomSystem.h>
#include <DirectXGame/application/base/Stage/CustomStage.h>



/// <summary>
/// カスタムシーン
/// </summary>
class CustomScene : public Engine::BaseScene
{
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update()   override;

	// 描画3d
	void Draw3D()   override;
	// 描画2d
	void Draw2D()   override;

private:
	Engine::Input* input = nullptr;
private:
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_ = nullptr;
	// エフェクト
	std::unique_ptr<EffectSystem> effect_ = nullptr;
	// シーン内の入力をまとめて管理する
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;
private:
	// カスタムUI
	std::unique_ptr<CustomUI> customUI_ = nullptr;
	// カスタムステージ
	std::unique_ptr<CustomStage> customStage_ = nullptr;
	// カスタムシステム
	std::unique_ptr<CustomSystem> customSystem_ = nullptr;
};
