#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "MoveData.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/Move/Base/LocomotionCoordinator.h"


/// <summary>
/// 移動に関するシステム
/// </summary>
class MoveSystem {
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
	void Update(const LocomotionContext& ctx, LocomotionCoordinator& coordinator, Engine::WorldTransform& world, InputSystem* input);
	// 更新(敵用)
	void UpdateEnemy(float dt);

private:
	// 速度処理
	void SpeedProcess(const LocomotionContext& ctx);
	// 状態処理
	void StateProcess();
	// アニメーション速度設定
	void AnimationSpeedProcess();
public:
	// 攻撃中の回転処理
	void AttackProcess(Engine::WorldTransform& world, const Vector3& direction);

public: // 取得
	// 移動データの取得
	MoveData& GetData() { return data_; }
	// 速度の取得
	Vector3 GetVelocity() const { return velocity_; }
	// 速度
	Vector3& Velocity() { return velocity_; }
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
private:
	// 移動データ
	MoveData data_{};
	// 速度
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	
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

