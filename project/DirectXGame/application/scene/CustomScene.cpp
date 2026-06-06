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
	// カメラ更新
	cameraManager_->Update();
}

void CustomScene::Draw3D() {
}

void CustomScene::Draw2D() {
}
