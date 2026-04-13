#include "RigidBodyComponent.h"

void Engine::RigidBodyComponent::Integrate(float deltaTime, WorldTransform& transform)
{
	if (isKinematic || isSleeping || inverseMass == 0.0f) return;

	// 並進運動の処理	
	ProcessTranslation(deltaTime,transform);

	// 回転運動の処理
	ProcessRotation(deltaTime, transform);

	// 変換行列の更新
	transform.Update();
}

void Engine::RigidBodyComponent::ProcessTranslation(float dt, WorldTransform& transform)
{
	// 加速度計算
	acceleration_ += force * inverseMass;

	// 重力適用
	if (useGravity) {
		// Y=Up なので、マイナス方向に重力をかける
		acceleration_.y += -gravity * gravityScale;
	}
	// 速度更新
	velocity_ += acceleration_ * dt;
	
	// 位置更新
	transform.translate_ += velocity_ * dt;

	// 力のリセット
	force = { 0, 0, 0 };
}

void Engine::RigidBodyComponent::ProcessRotation(float dt, WorldTransform& transform)
{
	// トルクによる回転（簡略化）
	Vector3 angularAcceleration = torque * inverseMass; // 実際は慣性モーメントが必要
	angularVelocity += angularAcceleration * dt;
	// 回転適用
	transform.rotate_ += angularVelocity * dt;

	// 力のリセット
	torque = { 0, 0, 0 };
}
