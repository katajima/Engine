#include "ResultScene.h"


// 初期化
void ResultScene::Initialize() {

	// inputSystem初期化
	inputSystem_ = std::make_unique<InputSystem>();
	inputSystem_->Initialize(GetInput());

	// リザルト用カメラ初期化
	resultCamera_ = std::make_unique<ResultCamera>();
	resultCamera_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables(), { 0,3.7f,-20.0f });
	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());
	cameraManager_->AddCamera({ resultCamera_.get(), true}, "resultCamera");

	GetEntityManager()->GetObject3dCommon()->SetDefaltCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());

	// エフェクト初期化
	effect_ = std::make_unique<Effect>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// ステージ初期化
	resultStage_ = std::make_unique<ResultStage>();
	resultStage_->Initialize(GetEntityManager(), GetGlobalVariables(), GetCamara());
	resultStage_->SetEffect(effect_.get());

	// リザルトUI初期化
	resultUI_ = std::make_unique<ResultUI>();
	resultUI_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());

	// リザルトシステム初期化
	resultSystem_ = std::make_unique<ResultSystem>();
	resultSystem_->Initialize(GetSceneManager(),inputSystem_.get(),
		GetEntityManager(), GetGlobalVariables());

};

// 終了
void ResultScene::Finalize() {
};

// 毎フレーム更新
void ResultScene::Update() {

	// 入力システム更新
	inputSystem_->Update(GetTime());

	// リザルトシステム更新
	resultSystem_->Update(GetTime());

	// ステージ更新
	resultStage_->Update(GetTime());

	// UI更新
	resultUI_->SetSelectedIndex(resultSystem_->GetSelectedIndex());
	resultUI_->Update(GetTime());

	// カメラ更新
	cameraManager_->Update();
};

// 描画3d
void ResultScene::Draw3D() {
};

// 描画2d
void ResultScene::Draw2D() {
	resultUI_->Draw();
};