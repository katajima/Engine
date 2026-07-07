#include "CustomScene.h"

void CustomScene::Initialize() {

	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	input = GetInput();
	// シーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(GetInput());

	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(nullptr, GetEntityManager(), GetGlobalVariables());
	

	// カスタムステージ初期化
	customStage_ = std::make_unique<CustomStage>();
	customStage_->Initialize(GetEntityManager(), cameraManager_.get());

	// カスタムシステム初期化
	customSystem_ = std::make_unique<CustomSystem>();
	customSystem_->Initialize(GetSceneManager(), inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	
	// カスタムUI初期化
	customUI_ = std::make_unique<CustomUI>();
	customUI_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());


	GetEntityManager()->GetObject3dCommon()->SetDefaultCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstanceManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());
}

void CustomScene::Finalize() {}

void CustomScene::Update() {
	// シーン内の入力を一度だけ更新する
	inputCoordinator_->Update(GetTime());

	// リザルトシステム更新
	customSystem_->Update(GetTime());

	// ステージ更新
	customStage_->Update(GetTime());

	// UI更新
	customUI_->Update(GetTime());

	// エフェクト更新
	effect_->Update(GetTime());

	// カメラ更新
	cameraManager_->Update();


	// カメラ更新
	cameraManager_->Update();
}

void CustomScene::Draw3D() {
}

void CustomScene::Draw2D() {

	//	UI描画
	customUI_->Draw();
}
