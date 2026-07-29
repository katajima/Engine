#include "RangeBombingCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void RangeBombingCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position)
{
	// 描画用カメラを生成し、範囲攻撃の広いエリアを描画できる距離を設定する。
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(provisionalData_.farClip_);
}

void RangeBombingCamera::Update()
{
	// 着弾地点が設定済みの場合は、その真上から見下ろす姿勢を作る。
	if (hasTargetPosition_) {
		Transform transform = uniqueCamera_->GetTransform();
		transform.translate = targetPosition_ + Vector3{ 0.0f, topDownHeight_, 0.0f };
		transform.rotate = { Math::DegreesToRadians(90.0f), 0.0f, 0.0f };
		uniqueCamera_->SetTransform(transform);
	}

	// カメラの行列を更新して描画へ反映する。
	uniqueCamera_->UpdateMatrix();
}

void RangeBombingCamera::SetTargetPosition(const Vector3& position)
{
	// 次回更新から指定位置を画面中央の着弾地点として使用する。
	targetPosition_ = position;
	hasTargetPosition_ = true;
}