#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "MoveData.h"
#include "DirectXGame/application/base/Input/InputSystem.h"

/// <summary>
/// 移動に関するシステム
/// </summary>
class MoveRequest {
public:
	// 移動状態(アニメーションやアニメーション速度変更用)
	enum class State {
		kIdle,		// 静止中
		kWalk,		// 歩き
		kRun,		// 走り
	};

	
	

	// 初期化
	void Initialize();
	// 更新(操作用)
	void Update(float dt, Engine::WorldTransform& world, InputSystem* input);
	// 更新(敵用)
	void UpdateEnemy(float dt);

private:
	// 速度処理
	void SpeedProcess(float dt);
	// 回転処理
	void RotateProcess(float dt, Engine::WorldTransform& world,const Vector3& velo);
	// 移動処理
	void MoveProcess(float dt, Engine::WorldTransform& world,Vector3& velo, bool isSpeed = true);
	// 状態処理
	void StateProcess();
	// アニメーション速度設定
	void AnimationSpeedProcess();
	// 方向処理
	void DirectionProcess(const Vector3& velo);
public:
	// カメラ基づく移動方向設定
	void CameraDirectionToMoveDirection(Vector3& velo);
	// 攻撃中の回転処理
	void AttackProcess(Engine::WorldTransform& world, const Vector3& direction);

public: // 取得
	// 移動データの取得
	MoveData& GetData() { return data_; }
	// 速度の取得
	Vector3 GetVelocity() const { return velocity_; }
	// 速度
	Vector3& Velocity() { return velocity_; }
	// 加速度
	Vector3& Acceleration() { return acceleration_; }
	// 方向の取得
	Vector3 GetDirection() const { return direction_; }
	// 状態の取得
	State GetState() const { return state_;}
	// アニメーション速度の取得
	float GetAnimationSpeed() const { return animationSpeed_; }

public: //設定
	// データ設定
	void SetData(const MoveData& data) { data_ = data; };

	// 移動可能かどうか設定
	void SetCanMove(bool canMove) { canMove_ = canMove; }
	// 空中かどうか設定
	void SetIsAir(bool isAir) { isAir_ = isAir; }
	// スピード設定
	void SetSpeed(float min, float max) {
		data_.minSpeed = min;
		data_.maxSpeed = max;
	};
	// 攻撃中か設定
	void SetIsAttack(bool is) { isAttack_ = is; };
	// カメラ設定
	void SetCamera(Engine::Camera* camera) { this->camera = camera; }
private: // 貰いもの
	// カメラ
	Engine::Camera* camera = nullptr;
	// 攻撃中か
	bool isAttack_ = false;
private:
	// 移動データ
	MoveData data_{};
	// 速度
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	// 移動方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };
	// 方向キープ
	Vector3 keepDirection_ = {};

	// 加速度
	Vector3 acceleration_ = { 0.0f,0.0f,0.0f };
	// 入力中かどうか 
	bool inputMove_ = false;
	// 入力方向
	Vector2 stickInput_ = { 0.0f,0.0f };
	// 移動速度
	float speed_ = 0.0f;
	// アニメーション速度
	float animationSpeed_ = 1.0f;
	// 状態
	State state_ = State::kIdle;
	// 空中か？
	bool isAir_ = false;
	// 移動可能フラグ
	bool canMove_ = true;
private:


};

