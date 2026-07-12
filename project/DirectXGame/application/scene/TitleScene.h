#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include<DirectXGame/engine/Effect/EffectComponent.h>
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/engine/base/Load/LoadLevelData.h"
#include"DirectXGame/application/base/Stage/TitleStage.h"
#include "DirectXGame/application/base/UI/TitleUI.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include"DirectXGame/application/base/Camera/Base/CameraManager.h"
#include "DirectXGame/application/base/Camera/FixedCamera/TitleCamera.h"
#include "DirectXGame/application/base/Weapon/Player/PlayerWeapon.h"


#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Animation/AnimationComponent.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public Engine::BaseScene
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
	// リソース初期化
	void InitializeResources();
	// カメラ初期化
	void InitializeCamera();
private:
	// タイトル画面のコントローラー入力とキーボード入力を統合して管理する。
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;
private:
	// ステージ
	std::unique_ptr<TitleStage> titleStage_ = nullptr;
	// UI
	std::unique_ptr<TitleUI> titleUI_ = nullptr;
	// レベルデータ
	std::unique_ptr<LoadLevelData> loadData_;
	// エフェクト
	std::unique_ptr<EffectSystem> effect_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_;
	// タイトルシーン用カメラ
	std::unique_ptr<TitleCamera> titleCamera_;
	// プレイヤー
	std::unique_ptr<ObjectComponent> objectComponent_ = nullptr;
	// 影用オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectComponentShadow_ = nullptr;

	// 武器
	std::unique_ptr<BaseWeapon> weapon_;
private:
	bool isStart_ = false;
	float startTimer_ = 0.0f;
};


