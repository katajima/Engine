#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"

/// <summary>
/// 移動コンポーネント
/// </summary>
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
	// 移動
	void AddMove(float deltaTime, bool is, WorldTransform& object)
	{
		if (is) {
			velocity_ += Acceleration(); // 加速度を速度に加算

			object.translate_ += GetVelocity() * deltaTime;
			if (Velocity().Length() != 0.0f) {
				direction_ = Velocity().Normalize();
			}
		}
	};

	// 着地処理
	void Landing(WorldTransform& world, RigidBodyComponent& rigid) {
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
	// 移動処理
	void Move(WorldTransform& world,Input* input) {
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
	// カメラ設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// 速度設定
	void SetSpeed(float speed) { speed_ = speed; };
	// ジャンプ回数現象
	void DecrementJumpCount() { jumpCount_--; }
	//	ジャンプ出来るか
	bool GetIsJump() const { return jumpCount_ > 0; }
	// 着地状態か
	bool GetIsLanding() const { return isLanding_; }
	// 最大ジャンプカウント設定
	void SetMaxJumpCount(int count) { jumpMaxCount_ = count; }

private:
	Camera* camera_ = nullptr;
	float groundHeight_ = -3.0f;		// 地面の高さ(デフォルトは0.0f、地面の高さを指定する場合に使用)
	Vector3 velocity_ = {};			// オブジェクトの速度
	Vector3 acceleration_ = {};		// オブジェクトの加速度
	Vector3 direction_{0,0.,-1.0f};	// 方向
	float speed_ = 1.0f;			// スピード
	int jumpMaxCount_ = 1;			// ジャンプ回数(最大)
	int jumpCount_ = 0;				// ジャンプ回数
	bool isLanding_ = false;		// 着地状態か

};