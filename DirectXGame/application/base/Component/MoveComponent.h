#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"

class MoveComponent
{
public:
	// 速度取得
	Vector3 GetVelocity() const { return velocity_; }
	// 速度
	Vector3& Velocity() { return velocity_; }

	// 加速度
	Vector3& Acceleration() { return acceleration_; }
	// 向いている方向
	Vector3 GetDirection() const { return direction_; }

	void AddMove(float deltaTime, bool is, Object3d& object)
	{
		if (is) {
			velocity_ += Acceleration(); // 加速度を速度に加算

			object.GetWorldTransform().translate_ += GetVelocity() * deltaTime;
			if (Velocity().Length() != 0.0f) {
				direction_ = Velocity();
			}
		}
	};

	// 着地処理
	void Landing(TransformComponent& world, RigidBodyComponent& rigid) {
		// 着地
		if (world.GetWorldPosition().y <= groundHeight_) {
			world.GetWorldTransform().translate_.y = groundHeight_;
			rigid.Velocity().y = 0.0f;
			rigid.SetIsGravity(false);
		}
		else {
			rigid.SetIsGravity(true);
		}
	}

private:
	float groundHeight_ = 0.0f;		// 地面の高さ(デフォルトは0.0f、地面の高さを指定する場合に使用)
	Vector3 velocity_ = {};			// オブジェクトの速度
	Vector3 acceleration_ = {};		// オブジェクトの加速度
	Vector3 direction_{0,0.,-1.0f};	// 方向
};