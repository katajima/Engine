#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"


#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"

#include<DirectXGame/engine/Effect/EffectComponent.h>


#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/base/Load/LoadLevelData.h"

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
	

	std::unique_ptr<Engine::Sprite> icon_B;
	std::unique_ptr<Engine::Sprite> title;


	Engine::Object3d tail;

	// ライト
	std::shared_ptr<Engine::DirectionalLight> directional;


	std::unique_ptr<LoadLevelData> loadData_;
};


