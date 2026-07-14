#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"

#include"DirectXGame/application/base/Camera/Base/CameraManager.h"
#include"DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"

#include "DirectXGame/engine/SkyBox/SkyBox.h"

class EffectScene : public Engine::BaseScene {
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
	// 物理入力、アクション入力、コマンドをまとめて管理する
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;

	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_;
	// エフェクト
	std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;
	// エフェクト
	std::unique_ptr<EffectSystem> effect_;
private:
	// スカイボックス
	std::unique_ptr<Engine::SkyBox> skyBox = nullptr;
	// 空
	Engine::Object3d* sky_ = nullptr;

	// 空スケール
	Vector3 skyBoxScale_ = { 100,100,100 };
};
