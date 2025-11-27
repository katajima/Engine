#include "MoveSystem.h"


void MoveSystem::Initialize() {
}

void MoveSystem::Update(float dt, WorldTransform& world, Input* input)
{
	Vector3 velo = GetVelocity();

	// スティックを動かした方向
	velo.x = input->GetGamePadLeftStick().x;
	velo.z = input->GetGamePadLeftStick().y;
	// 入力方向を正規化
	velo = Normalize(velo);

	// スティック入力を保存
	stickInput_ = input->GetGamePadLeftStick();

	// 移動処理
	MoveProcess(dt, world, velo);
}

void MoveSystem::Update(float dt, WorldTransform& world)
{
	Vector3 velo = GetVelocity();
	velo = Normalize(velo);

	// 移動処理
	MoveProcess(dt, world, velo);
}


#pragma region Process

void MoveSystem::SpeedProcess(float dt)
{

	if (data_.moveType == MoveType::LINEAR) { // 一定
		// スピードをだんだん上げる
		data_.speed = data_.maxSpeed;
	}
	else if (data_.moveType == MoveType::ACCELERATE) { // 加速
		// 目標は maxSpeed
		float targetSpeed = data_.maxSpeed;

		// dt を使って徐々に近づける
		// speedAcceleration は「1秒あたりどれくらい寄せるか」
		float t = Math::Clamp(data_.speedAcceleration,0.0f, 1.0f);

		// 現在の speed から targetSpeed へ補間していく
		data_.speed = Lerp(data_.speed, targetSpeed, t);

		// 最低/最高速度の範囲に収める
		data_.speed = Math::Clamp(data_.speed,data_.minSpeed, data_.maxSpeed);
	}

	// スティックの倒し方に応じてスピードを変化させる
	if (data_.isStickToSpeed) {
		float stickLen = Math::Clamp(std::abs(stickInput_.Length()),0.0f, 1.0f);
		data_.speed *= stickLen;
	}

}

void MoveSystem::RotateProcess(float dt, WorldTransform& world, const Vector3& velo)
{
	// 移動ベクトルがゼロなら回転処理しない
	if (velo.Length() == 0.0f) return;

	// 目標方向（X=Right, Y=Up, Z=Forward）
	float targetYaw = std::atan2(velo.x, velo.z);

	float& currentYaw = world.rotate_.y;

	// ----------------------------
	// 角度差を [-π, π] に正規化
	// ----------------------------
	float delta = targetYaw - currentYaw;
	while (delta > DirectX::XM_PI)  delta -= DirectX::XM_2PI;
	while (delta < -DirectX::XM_PI)  delta += DirectX::XM_2PI;

	// ----------------------------
	// 補間（rotationSpeed は 0〜1 の割合）
	// rotationSpeed = 0.1f なら 10% だけ近づく
	// ----------------------------
	currentYaw += delta * data_.rotationSpeed;
}

void MoveSystem::MoveProcess(float dt, WorldTransform& world, Vector3& velo)
{
	// 動いているなら
	if (velo.x != 0.0f || velo.z != 0.0f) {
		// 入力方向を正規化
		velo = Normalize(velo);

		// スピード処理
		SpeedProcess(dt);

		// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
		if (camera_) {
			Matrix4x4 cameraWorldMatrix = Inverse(camera_->GetViewMatrix());

			// カメラの向きに基づいて移動方向をワールド座標系に変換
			Vector3 worldDirection = {
				velo.x * cameraWorldMatrix.m[0][0] + velo.z * cameraWorldMatrix.m[2][0],
				0.0f,
				velo.x * cameraWorldMatrix.m[0][2] + velo.z * cameraWorldMatrix.m[2][2]
			};

			velo = Multiply(Normalize(worldDirection), data_.speed);
		}
		else {
			// 移動方向にスピードを掛ける
			velo = Multiply(velo, data_.speed);
		}

		// 回転処理
		RotateProcess(dt, world, velo);
	
		inputMove_ = true;
	}
	else {
		inputMove_ = false;
	}

	// 移動速度をセット
	velocity_.x = velo.x;
	velocity_.z = velo.z;

	// 移動方向を正規化して保存
	direction_ = velocity_.Normalize();


	// 移動可能かどうか
	if (data_.canMove) {
		world.translate_ += Multiply(velocity_, dt);
	}
}


#pragma endregion // 処理


