#include "StageCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
void StageCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position) {
	this->entityManager = entityManager;	// エンティティ3d

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(Engine::Camera::kDefaultFarClip);	// 共通のFarクリップ距離を設定

	// カメラ位置と回転設定
	uniqueCamera_->SetTranslate(position);
	uniqueCamera_->SetRotate({ Math::DegreesToRadians(60),0,0 });

	offsetPos = { 0,0,-2000 };
}

void StageCamera::Update() {
	const float kRotateSpeedX = 0.002f;

	Transform transform = uniqueCamera_->GetTransform();

	transform.rotate.y -= kRotateSpeedX;


	// 回転適用 → カメラ位置を計算
	Matrix4x4 rotY = MakeRotateYMatrix(uniqueCamera_->GetRotate().y);
	Matrix4x4 rotX = MakeRotateXMatrix(uniqueCamera_->GetRotate().x);
	Matrix4x4 rotateMatrix = rotX * rotY;
	Vector3 offset = TransformNormal(offsetPos, rotateMatrix);

	Vector3 targetPos = anchorPoint;
	Vector3 desiredCameraPos = Add(targetPos, offset);

	transform.translate = desiredCameraPos;
	uniqueCamera_->SetTransform(transform);
	uniqueCamera_->UpdateMatrix();
}
