#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"


/// <summary>
/// 移動に関するシステム
/// </summary>
class MoveSystem {
public:

	// 移動タイプ
	enum class MoveType
	{
		LINEAR,		// 線形移動
		ACCELERATE, // 加速移動
		DECELERATE, // 減速移動
	};

	// 移動状態(アニメーションやアニメーション速度変更用)
	enum class State {
		kIdle,		// 静止中
		kWalk,		// 歩き
		kRun,		// 走り
		kStrafe,	// 横移動
	};

	// 


	// 移動データ
	struct Data
	{
		// 移動速度
		float speed = 0.0f;
		// 移動可能フラグ
		bool canMove = true;
		// 加速度
		float speedAcceleration = 0.1f;
		// 現在の速度
		float currentSpeed = 0.0f;
		// 最大速度
		float maxSpeed = 0.0f;
		// 最小速度
		float minSpeed = 0.0f;
		// 移動タイプ
		MoveType moveType = MoveType::LINEAR;
		// 回転補間速度
		float rotationSpeed = 0.1f;
		// スティックの倒し方で速度を変化するかどうか
		bool isStickToSpeed = true;
	};

	// 初期化
	void Initialize();
	// 更新(操作用)
	void Update(float dt,WorldTransform& world, Input* input);
	// 更新(非操作用)
	void Update(float dt, WorldTransform& world);

	// 移動可能かどうか設定
	void SetCanMove(bool canMove) { data_.canMove = canMove; }

	// スピード設定
	void SetSpeed(float min, float max) { 
		data_.minSpeed = min; 
		data_.maxSpeed = max;
	};


private:
	// 速度処理
	void SpeedProcess(float dt);
	// 回転処理
	void RotateProcess(float dt, WorldTransform& world,const Vector3& velo);
	// 移動処理
	void MoveProcess(float dt, WorldTransform& world,Vector3& velo);

public: // 取得
	// 移動データの取得
	Data& GetData() { return data_; }
	// 速度の取得
	Vector3 GetVelocity() const { return velocity_; }
	// 速度
	Vector3& Velocity() { return velocity_; }
	// 加速度
	Vector3& Acceleration() { return acceleration_; }
	// 方向の取得
	Vector3 GetDirection() const { return direction_; }

public: //設定
	// カメラ設定
	void SetCamera(Camera* camera) { camera_ = camera; }
private: // 貰いもの
	// カメラ
	Camera* camera_ = nullptr;
private:
	// 移動データ
	Data data_;
	// 速度
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	// 移動方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };
	// 加速度
	Vector3 acceleration_ = { 0.0f,0.0f,0.0f };
	// 入力中かどうか 
	bool inputMove_ = false;
	// 入力方向
	Vector2 stickInput_ = { 0.0f,0.0f };
private:


};

