#include "SelectScene.h"


void SelectScene::Initialize() {
	input = GetInput();
	// 入力システム初期化
	inputSystem_ = std::make_unique<InputSystem>();
	inputSystem_->Initialize(GetInput());

	// セレクトカメラ生成
	selectCamera_ = std::make_unique<SelectCamera>();
	selectCamera_->Initialize(nullptr, GetEntityManager(), GetGlobalVariables(), { 1.8f,0.7f,3.3f });
	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(nullptr, GetEntityManager(), GetGlobalVariables());
	cameraManager_->AddCamera({ selectCamera_.get() ,true }, "selectCamera");

	GetEntityManager()->GetObject3dCommon()->SetDefaltCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());


	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// ステージ生成
	selectStage_ = std::make_unique<SelectStage>();
	selectStage_->Initialize(GetEntityManager(), cameraManager_.get());
	selectStage_->SetEffect(effect_.get());
	selectStage_->GetPlayerCar()->GetStateMachine()->ChangeState(CarMainState::SelectMove);
	// UI生成
	selectUI_ = std::make_unique<SelectUI>();
	selectUI_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());

	// セレクトシーンの管理クラス生成
	selectSystem_ = std::make_unique<SelectSystem>();
	selectSystem_->Initialize(GetSceneManager(), inputSystem_.get(), GetEntityManager(), GetGlobalVariables());

}


// 終了
void SelectScene::Finalize() {}

// 毎フレーム更新
void SelectScene::Update() {
	// 入力システム更新
	inputSystem_->Update(GetTime());
	// セレクトシーンの管理クラス更新
	selectSystem_->Update(GetTime());

	

	selectStage_->SetRatio(GetWinApp()->GetCalculateWindowToFullscreenScaleRatioWidth());
	// ステージ更新
	selectStage_->Update(GetTime());
	// UI更新
	selectUI_->SetSelectedIndex(selectSystem_->GetSelectedIndex());
	selectUI_->Update(GetTime());
	// エフェクト
	effect_->Update(GetTime());
	// カメラ更新
	cameraManager_->Update();
}

// 描画
void SelectScene::Draw3D() {
	// ステージ描画
	selectStage_->Draw();
}

void SelectScene::Draw2D() {
	// UI描画
	selectUI_->Draw();
}