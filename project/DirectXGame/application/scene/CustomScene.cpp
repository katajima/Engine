#include "CustomScene.h"

void CustomScene::Initialize() {

	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	input = GetInput();
	// 入力システム初期化
	inputSystem_ = std::make_unique<InputSystem>();
	inputSystem_->Initialize(GetInput());

	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(nullptr, GetEntityManager(), GetGlobalVariables());
	

	// カスタムステージ初期化
	customStage_ = std::make_unique<CustomStage>();
	customStage_->Initialize(GetEntityManager(), cameraManager_.get());

	// カスタムシステム初期化
	customSystem_ = std::make_unique<CustomSystem>();
	customSystem_->Initialize(GetSceneManager(), inputSystem_.get(), GetEntityManager(), GetGlobalVariables());
	
	// カスタムUI初期化
	customUI_ = std::make_unique<CustomUI>();
	customUI_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());


	GetEntityManager()->GetObject3dCommon()->SetDefaltCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());
}

void CustomScene::Finalize() {}

void CustomScene::Update() {
	// 入力システム更新
	inputSystem_->Update(GetTime());





	// 入力システム更新
	inputSystem_->Update(GetTime());

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
