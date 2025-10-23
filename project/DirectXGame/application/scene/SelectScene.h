#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/base/BaseClass/Character/BaseCharacterManeger.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"

#include"DirectXGame/application/base/Stage/Stage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/BaseClass/Bullet/BulletManager.h"
#include"DirectXGame/application/base/BaseClass/Camera/CameraManeger.h"

#include "DirectXGame/application/base/Input/InputHander.h"

/// <summary>
/// セレクトシーン
/// </summary>
class SelectScene : public BaseScene
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

};

