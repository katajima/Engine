#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include<DirectXGame/engine/Effect/EffectComponent.h>
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/engine/base/Load/LoadLevelData.h"
#include"DirectXGame/application/base/Stage/TitleStage.h"
#include "DirectXGame/application/base/UI/TitleUI.h"

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
private:
	// リソース初期化
	void InitializeResources();
	// カメラ初期化
	void InitializeCamera();
private:
	Engine::Input* input_ = nullptr;
	Engine::Audio* audio_ = nullptr;
	// カメラ
	std::unique_ptr <Engine::Camera> camera;
private:
	// ステージ
	std::unique_ptr<TitleStage> titleStage_ = nullptr;
	// UI
	std::unique_ptr<TitleUI> titleUI_ = nullptr;
	// レベルデータ
	std::unique_ptr<LoadLevelData> loadData_;
};


