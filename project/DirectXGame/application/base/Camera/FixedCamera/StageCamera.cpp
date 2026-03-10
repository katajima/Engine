#include "StageCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
void StageCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position) {
	this->entityManager = entityManager;	// エンティティ3d

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->farClip_ = 10000.0f;

	// カメラ位置と回転設定
	uniqueCamera_->transform_.translate = position;
	uniqueCamera_->transform_.rotate = { Math::DegreesToRadians(60),0,0 };


	offsetPos = { 0,0,-2000 };
}

void StageCamera::Update() {
	const float kRotateSpeedX = 0.002f;

	uniqueCamera_->transform_.rotate.y -= kRotateSpeedX;


	// 回転適用 → カメラ位置を計算
	Matrix4x4 rotY = MakeRotateYMatrix(uniqueCamera_->transform_.rotate.y);
	Matrix4x4 rotX = MakeRotateXMatrix(uniqueCamera_->transform_.rotate.x);
	Matrix4x4 rotateMatrix = rotX * rotY;
	Vector3 offset = TransformNormal(offsetPos, rotateMatrix);

	Vector3 targetPos = anchorPoint;
	Vector3 desiredCameraPos = Add(targetPos, offset);

	uniqueCamera_->transform_.translate = desiredCameraPos;

	uniqueCamera_->UpdateMatrix();
}
