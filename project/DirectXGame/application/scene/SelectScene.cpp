#include "SelectScene.h"


void SelectScene::Initialize() {
	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(nullptr, GetEntity3DManager(), GetGlobalVariables());
	//cameraManager_->AddCamera({ titleCamera_.get(), true }, "titleCamera");

	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(cameraManager_->GetCamera());
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntity3DManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());

	// ステージ生成
	selectStage_ = std::make_unique<SelectStage>();
	selectStage_->Initialize(GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), GetCamara());
	// UI生成
	selectUI_ = std::make_unique<SelectUI>();
	selectUI_->Initialize(nullptr, GetEntity2DManager(), GetGlobalVariables());
}


// 終了
void SelectScene::Finalize() {}

// 毎フレーム更新
void SelectScene::Update() {


	// ステージ更新
	selectStage_->Update(GetTime());
	// UI更新
	selectUI_->Update(GetTime());
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