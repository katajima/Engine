#include "MoveComponent.h"

void MoveComponent::AddMove(float deltaTime, bool is, WorldTransform& object)
{
	if (is) {
		velocity_ += Acceleration(); // 加速度を速度に加算

		object.translate_ += GetVelocity() * deltaTime;
		if (Velocity().Length() != 0.0f) {
			direction_ = Velocity().Normalize();
		}
	}
}
void MoveComponent::Landing(WorldTransform& world, RigidBodyComponent& rigid)
{
	// 着地
	if (world.GetWorldPosition().y <= groundHeight_) {
		world.translate_.y = groundHeight_;
		rigid.Velocity().y = 0.0f;
		rigid.SetIsGravity(false);

		jumpCount_ = jumpMaxCount_;
		isLanding_ = true;
	}
	else {
		rigid.SetIsGravity(true);
		isLanding_ = false;
	}
}

void MoveComponent::Move(WorldTransform& world, Input* input)
{
	Vector3 velo = GetVelocity();


	velo.x = input->GetGamePadLeftStick().x;
	velo.z = input->GetGamePadLeftStick().y;


	if (velo.x != 0.0f || velo.z != 0.0f) {


		// 入力方向を正規化
		velo = Normalize(velo);
		velo = Multiply(velo, speed_);


		// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
		if (camera_) {
			Matrix4x4 cameraWorldMatrix = Inverse(camera_->GetViewMatrix());

			// カメラの向きに基づいて移動方向をワールド座標系に変換
			Vector3 worldDirection = {
				velo.x * cameraWorldMatrix.m[0][0] + velo.z * cameraWorldMatrix.m[2][0],
				0.0f,
				velo.x * cameraWorldMatrix.m[0][2] + velo.z * cameraWorldMatrix.m[2][2]
			};

			velo = Multiply(Normalize(worldDirection), speed_);
		}

		// スティックを動かしてたら
		if (velo.Length() != 0) {
			world.rotate_.y = std::atan2(velo.x, velo.z);
		}
	}
	Velocity().x = velo.x;
	Velocity().z = velo.z;
}
