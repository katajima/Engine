#pragma once
#include "MoveData.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/Character/Move/Base/LocomotionCoordinator.h"


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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新(操作用)
	/// </summary>
	void Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator);
	/// <summary>
	/// 更新(敵用)
	/// </summary>
	void UpdateEnemy(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator);

private:
	/// <summary>
	/// 速度処理
	/// </summary>
	void SpeedProcess(const Character::CharacterContext& ctx);
	/// <summary>
	/// 状態処理
	/// </summary>
	void StateProcess();
	/// <summary>
	/// アニメーション速度設定
	/// </summary>
	void AnimationSpeedProcess();
public: // 取得
	/// <summary>
	/// 移動データの取得
	/// </summary>
	MoveData& Data() { return data_; }
	//
	MoveData GetData() const { return data_; }
	/// <summary>
	/// 速度の取得
	/// </summary>
	Vector3 GetVelocity() const { return velocity_; }
	/// <summary>
	/// 状態の取得
	/// </summary>
	State GetState() const { return state_;}
	/// <summary>
	/// アニメーション速度の取得
	/// </summary>
	float GetAnimationSpeed() const { return animationSpeed_; }
public: //設定
	/// <summary>
	/// データ設定
	/// </summary>
	void SetData(const MoveData& data) { data_ = data; };
	/// <summary>
	/// スピード設定
	/// </summary>
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
private:


};

