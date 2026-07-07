#include "ResultScene.h"


// 初期化
void ResultScene::Initialize() {

	// シーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(GetInput());

	// リザルト用カメラ初期化
	resultCamera_ = std::make_unique<ResultCamera>();
	resultCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), { 0,3.7f,-20.0f });
	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	cameraManager_->AddCamera({ resultCamera_.get(), true}, "resultCamera");

	GetEntityManager()->GetObject3dCommon()->SetDefaltCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());

	// エフェクト初期化
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// ステージ初期化
	resultStage_ = std::make_unique<ResultStage>();
	resultStage_->Initialize(GetEntityManager(), cameraManager_.get());
	resultStage_->SetEffect(effect_.get());

	// リザルトUI初期化
	resultUI_ = std::make_unique<ResultUI>();
	resultUI_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());

	// リザルトスコアUI初期化
	resultScoreUI_ = std::make_unique<ResultScoreUI>();
	resultScoreUI_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());

	overUI_ = std::make_unique<Engine::Sprite>();
	overUI_->Initialize(GetEntityManager()->GetSpriteCommon(), "resources/Texture/text/over.dds");
	overUI_->SetPosition({640,200});
	overUI_->SetAnchorPoint({0.5f,0.5f });
	//overUI_->SetTextureSize({256,64});
	overUI_->SetSize({ 256,64 });

	// リザルトシステム初期化
	resultSystem_ = std::make_unique<ResultSystem>();
	resultSystem_->Initialize(GetSceneManager(),inputCoordinator_->GetInputSystem(),
		GetEntityManager(), GetGlobalVariables());

};

// 終了
void ResultScene::Finalize() {
};

// 毎フレーム更新
void ResultScene::Update() {

	// 入力システム更新
	inputCoordinator_->Update(GetTime());

	// リザルトシステム更新
	resultSystem_->Update(GetTime());

	// ステージ更新
	resultStage_->Update(GetTime());

	// UI更新
	resultUI_->SetSelectedIndex(resultSystem_->GetSelectedIndex());
	resultUI_->Update(GetTime());

	// UI更新
	resultScoreUI_->SetScore(GetSceneData().score);
	resultScoreUI_->Update(GetTime());

	overUI_->Update();

	effect_->Update(GetTime());

	// カメラ更新
	cameraManager_->Update();
};

// 描画3d
void ResultScene::Draw3D() {
};

// 描画2d
void ResultScene::Draw2D() {
	resultUI_->Draw();

	resultScoreUI_->Draw();

	if (GetSceneData().playerDie) {
		overUI_->Draw();
	}
};
