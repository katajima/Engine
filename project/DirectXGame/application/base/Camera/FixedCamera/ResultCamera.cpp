#include "ResultCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"


void ResultCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position){
	this->entityManager = entityManager;	// エンティティ
	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(Engine::Camera::kDefaultFarClip);	// 共通のFarクリップ距離を設定

	// カメラ位置と回転設定
	uniqueCamera_->SetTranslate(position);
	uniqueCamera_->SetRotate({ Math::DegreesToRadians(7),0,0 });
}

void ResultCamera::Update() {
	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}



