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
	};

	
	// 移動データ
	struct Data
	{
		// 移動可能フラグ
		bool canMove = true;
		// 加速度
		float speedAcceleration = 0.1f;
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
		// 空中での速度制限をするかどうか
		bool isLimitAirSpeed = true;
		// 空中での速度係数 
		float airSpeedRate = 0.85f;

		// 歩きから走り状態への境界指定
		float moveStateThreshold = 10.0f; // 移動状態のしきい値

		// 歩きアニメーション最小速度
		float animetionSpeedMinWalk = 0.1f; 
		// 歩きアニメーション最大速度
		float animetionSpeedMaxWalk = 2.0f;
		
		// 走りアニメーション最小速度
		float animetionSpeedMinRun = 1.0f;
		// 走りアニメーション最大速度
		float animetionSpeedMaxRun = 3.0f;

		// 走りと歩きで同じアニメーションか
		bool isSameAnimation = true;
	};

	// 初期化
	void Initialize();
	// 更新(操作用)
	void Update(float dt,WorldTransform& world, Input* input);
	// 更新(非操作用)
	void Update(float dt, WorldTransform& world);
	// 更新(敵用)
	void UpdateEnemy(float dt, WorldTransform& world);

	// isAttackMove(
	void UpdateAttack(float dt, WorldTransform& world);

private:
	// 速度処理
	void SpeedProcess(float dt);
	// 回転処理
	void RotateProcess(float dt, WorldTransform& world,const Vector3& velo);
	// 移動処理
	void MoveProcess(float dt, WorldTransform& world,Vector3& velo, bool isSpeed = true);
	// 状態処理
	void StateProcess();
	// アニメーション速度設定
	void AnimationSpeedProcess();
	// 方向処理
	void DirectionProcess(const Vector3& velo);
	// ダッシュ時の処理
	void DashProcess(WorldTransform& world);
public:
	// カメラ基づく移動方向設定
	void CameraDirectionToMoveDirection(Vector3& velo);
	// 攻撃中の回転処理
	void AttackProcess(WorldTransform& world, const Vector3& direction);

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
	// 状態の取得
	State GetState() const { return state_;}
	// アニメーション速度の取得
	float GetAnimationSpeed() const { return animationSpeed_; }

public: //設定
	// 移動可能かどうか設定
	void SetCanMove(bool canMove) { data_.canMove = canMove; }
	// 空中かどうか設定
	void SetIsAir(bool isAir) { isAir_ = isAir; }
	// スピード設定
	void SetSpeed(float min, float max) {
		data_.minSpeed = min;
		data_.maxSpeed = max;
	};
	// ダッシュしているか設定
	void SetIsDash(bool isDash) { isDash_ = isDash; }
	// 攻撃中か設定
	void SetIsAttack(bool is) { isAttack_ = is; };
	// 攻撃中か設定
	void SetIsAttackCanMove(bool is) { isAttackCanMove_ = is; };

	// カメラ設定
	void SetCamera(Camera* camera) { camera_ = camera; }
private: // 貰いもの
	// カメラ
	Camera* camera_ = nullptr;

	// ダッシュしているかどうか
	bool isDash_ = false;
	// 攻撃中か
	bool isAttack_ = false;
	// 攻撃痛に動かせるか
	bool isAttackCanMove_ = false;
private:
	// 移動データ
	Data data_{};
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
private:


};

