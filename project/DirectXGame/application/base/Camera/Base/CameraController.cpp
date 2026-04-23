#include "CameraController.h"

void CameraController::Initialize(Engine::Camera* camera, const InputSystem* input) {
	this->camera = camera;
	this->input = input;

	// 追従
	follow = std::make_unique<CameraFollow>();
	// 回転
	rotation = std::make_unique<CameraRotation>();
	// ロックオン
	lockOn = std::make_unique<CameraLockOn>();
	// 

}


void CameraController::Update(float dt) {
	Transform transform = camera->GetTransform();


	follow->Update(dt);



	camera->SetTransform(transform);
};