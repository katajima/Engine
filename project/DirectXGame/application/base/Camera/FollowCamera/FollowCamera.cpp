#include "FollowCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void FollowCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position)
{
	// インプット
	this->inputSystem = inputSystem;

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(provisionalData_.farClip_);

	// カメラ回転設定
	uniqueCamera_->SetRotate(provisionalData_.rotate);
	uniqueCamera_->AddEffectBlock("bloom", Engine::PostEffectBlockType::kBloom);
	uniqueCamera_->GetPostEffectPass(0)->GetPostEffectData()->GetBloom()->Data()->intensity = provisionalData_.bloomIndensity;
	uniqueCamera_->GetPostEffectPass(1)->GetPostEffectData()->GetGaussian()->Data()->num = provisionalData_.gaussianNum;
	uniqueCamera_->GetPostEffectPass(1)->GetPostEffectData()->GetGaussian()->Data()->sigma = provisionalData_.gaussianSigma;

	// 操作
	controller = std::make_unique<CameraController>();
	controller->Initialize(uniqueCamera_.get(), inputSystem);

	baseOffset.y = 10.0f;
	baseOffset.z = -25;
}

void FollowCamera::Update() {
	controller->SetTarget(target_);
	controller->SetLockOnTarget(target);
	// カメラを使っているなら
	if (useCamera) {
		uniqueCamera_->GetPostEffectManager()->AddPipeline(uniqueCamera_->GetPostEffectPipeline());
	}
	controller->Update(Engine::MyGame::GameTime());
	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
