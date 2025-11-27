#include "RigidBodyComponent.h"

void RigidBodyComponent::Integrate(float deltaTime, WorldTransform& transform)
{
	if (isKinematic || isSleeping || inverseMass == 0.0f) return;

	// 加速度計算
	Vector3 acceleration = force * inverseMass;

	// 速度更新
	velocity_ += acceleration /** deltaTime*/;

	// 重力適用
	if (useGravity) {
		velocity_.y += -gravity * gravityScale;
	}

	// 位置更新
	transform.translate_ += velocity_ * deltaTime;



	// トルクによる回転（簡略化）
	Vector3 angularAcceleration = torque * inverseMass; // 実際は慣性モーメントが必要
	angularVelocity += angularAcceleration * deltaTime;

	// 回転適用
	transform.rotate_ += angularVelocity * deltaTime;


	// 力のリセット
	force = { 0, 0, 0 };
	torque = { 0, 0, 0 };
	transform.Update();
}
