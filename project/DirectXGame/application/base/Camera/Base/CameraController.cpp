#include "CameraController.h"

void CameraController::Initialize(Engine::Camera* camera, const InputSystem* input) {
	this->camera = camera;
	this->input = input;

	// 追従
	follow = std::make_unique<CameraFollow>();
	follow->Initialize();
	// 回転
	rotation = std::make_unique<CameraRotation>();
	rotation->Initialize();
	// ロックオン
	lockOn = std::make_unique<CameraLockOn>();
	// 
	collision = std::make_unique<CameraCollision>();


}


void CameraController::Update(float dt) {
	Transform transform = camera->GetTransform();
	
	if (lockOn->GetData().isLockOn) {
		transform.rotate = lockOn->Update(dt);
	}
	else {
		rotation->Update(transform,input,dt);
	}

	follow->Update(transform,dt);

	collision->Update(transform, dt);

	camera->SetTransform(transform);
};

void CameraController::SetTraget(const Engine::WorldTransform* target) { 
	this->target = target;
	follow->SetTraget(target);
	collision->SetTraget(target);
};
