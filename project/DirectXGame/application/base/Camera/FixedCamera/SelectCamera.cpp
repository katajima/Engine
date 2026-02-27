#include "SelectCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void SelectCamera::Initialize(InputSystem* inputSystem, Engine::Entity3DManager* entity3DManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position){
	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
	uniqueCamera_->farClip_ = 10000.0f;

	// カメラ位置と回転設定
	uniqueCamera_->transform_.translate = position;
	uniqueCamera_->transform_.rotate = { -0.128f,-0.460f,0 };

};


void SelectCamera::Update() {
	uniqueCamera_->UpdateMatrix();
}