#include "TitleCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void TitleCamera::Initialize(InputSystem* inputSystem, Engine::Entity3DManager* entity3DManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position){
	this->entity3DManager = entity3DManager;	// エンティティ3d

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
	uniqueCamera_->farClip_ = 10000.0f;

	// カメラ位置と回転設定
	uniqueCamera_->transform_.translate = position;
	uniqueCamera_->transform_.rotate = {Math::DegreesToRadians(7),0,0};


}

void TitleCamera::Update(){
	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
