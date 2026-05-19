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
	// 判定
	collision = std::make_unique<CameraCollision>();
	// シェイク
	shake = std::make_unique<CameraShake>();
	shake->Initialize(camera);
	// ズーム
	zoom = std::make_unique<CameraZoom>();
	zoom->SetDefultZ(camera->GetTransform().scale.z);

}


void CameraController::Update(float dt) {
	// カメラトランスフォーム取得
	Transform transform = camera->GetTransform();
	
	// ロックオンしているなら
	if (lockOn->GetData().isLockOn) {
		transform.rotate = lockOn->Update(transform, dt);
	}
	else { // していないなら回転処理を優先
		// 回転処理
		rotation->Update(transform,input,dt);
	}
	// 追従
	follow->Update(transform,dt);
	// ズーム
	zoom->Update(transform.scale.z,dt);
	// シェイク
	shake->SetDefultPos(transform.translate);
	shake->Update(transform.translate,dt);
	// 判定
	collision->Update(transform, dt);
	// カメラ位置決定
	camera->SetTransform(transform);
};

void CameraController::SetTraget(const Engine::WorldTransform* target) { 
	this->target = target;
	follow->SetTraget(target);
	collision->SetTraget(target);
};

void CameraController::SetLockOnTarget(const Engine::WorldTransform* target) {
	this->lockOnTarget = target;
	lockOn->GetData().target = target;
	lockOn->GetData().isLockOn = target != nullptr;
}
