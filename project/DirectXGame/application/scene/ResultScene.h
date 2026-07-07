#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include "DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/audio/Audio.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"

#include<DirectXGame/engine/Effect/EffectComponent.h>

#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/base/Load/LoadLevelData.h"

#include "DirectXGame/application/base/UI/ResultUI.h"
#include "DirectXGame/application/base/UI/ResultScoreUI.h"

#include "DirectXGame/application/base/Stage/ResultStage.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include "DirectXGame/application/base/Camera/FixedCamera/ResultCamera.h"
#include "DirectXGame/application/base/Game/Result/ResultSystem.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"
class ResultScene : public Engine::BaseScene {
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

private:
	// シーン内の入力をまとめて管理する
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;
	// エフェクト
	std::unique_ptr<EffectSystem> effect_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_;
	// リザルトカメラ
	std::unique_ptr<ResultCamera> resultCamera_;
	// リザルトステージ
	std::unique_ptr<ResultStage> resultStage_;
	// リザルトUI
	std::unique_ptr<ResultUI> resultUI_;
	// リザルトスコアUI
	std::unique_ptr<ResultScoreUI> resultScoreUI_;
	// リザルトシステム
	std::unique_ptr<ResultSystem> resultSystem_;
	//
	std::unique_ptr<Engine::Sprite> overUI_;
};
