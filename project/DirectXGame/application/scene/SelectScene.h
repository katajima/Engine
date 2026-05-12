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

#include "DirectXGame/application/base/Input/InputHander.h"
#include "DirectXGame/application/base/Stage/SelectStage.h"
#include "DirectXGame/application/base/UI/SelectUI.h"
#include "DirectXGame/application/base/Camera/FixedCamera/SelectCamera.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Game/Select/SelectSystem.h"
#include <DirectXGame/application/base/Input/InputSystem.h>

/// <summary>
/// セレクトシーン
/// </summary>
class SelectScene : public Engine::BaseScene
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
	// セレクトシーン用ステージ
	std::unique_ptr<SelectStage> selectStage_ = nullptr;
	// セレクトシーン用UI
	std::unique_ptr<SelectUI> selectUI_ = nullptr;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_ = nullptr;
	// セレクトカメラ
	std::unique_ptr<SelectCamera> selectCamera_ = nullptr;
	// エフェクト
	std::unique_ptr<EffectSystem> effect_ = nullptr;
	// セレクトシーンの管理クラス
	std::unique_ptr<SelectSystem> selectSystem_ = nullptr;
	// インプットシステム
	std::unique_ptr<InputSystem> inputSystem_ = nullptr;
};

