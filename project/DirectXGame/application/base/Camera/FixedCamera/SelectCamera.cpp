#include "SelectCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void SelectCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position){
	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(10000.0f);

	// カメラ位置と回転設定
	uniqueCamera_->SetTranslate(position);
	uniqueCamera_->SetRotate({ -0.128f,-0.460f,0 });

};


void SelectCamera::Update() {
	uniqueCamera_->UpdateMatrix();
}