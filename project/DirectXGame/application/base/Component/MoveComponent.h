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
	void AddMove(float deltaTime, bool is, WorldTransform& object);

	// 着地処理
	void Landing(WorldTransform& world, RigidBodyComponent& rigid);
	// 移動処理
	void Move(WorldTransform& world, Input* input);
	// 移動処理
	void Move(WorldTransform& world);


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