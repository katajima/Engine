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
	uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(0)->GetPostEffectData()->GetBloom()->Data()->intensity = provisionalData_.bloomIndensity;
	uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(1)->GetPostEffectData()->GetGaussian()->Data()->num = provisionalData_.gaussianNum;
	uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(1)->GetPostEffectData()->GetGaussian()->Data()->sigma = provisionalData_.gaussianSigma;

	// 操作
	controller = std::make_unique<CameraController>();
	controller->Initialize(uniqueCamera_.get(), inputSystem);


	baseOffset.z = -22.5;
}

void FollowCamera::Update() {
	controller->SetTraget(target_);
	controller->SetLockOnTarget(target);
	// カメラを使っているなら
	if (useCamera) {
		uniqueCamera_->GetPostEffectManager()->AddEffectBlocks(uniqueCamera_->GetPostEffectBlocks());
	}
	controller->Update(Engine::MyGame::GameTime());
	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
