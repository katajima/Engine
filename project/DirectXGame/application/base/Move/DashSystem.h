#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"


/// <summary>
/// ダッシュに関するシステムクラス
/// </summary>
class DashSystem {
public:

	// ダッシュ用データ
	struct Data {
		// ダッシュ初速度
		float startSpeed = 100.0f;
		// 加速
		float acceleration = 0.0f;
		// 減衰
		float friction = 10.0f;
		// 最大ダッシュ時間
		float maxTime = 0.15f;
		// ダッシュ中に重力適用させるかのフラグ
		bool isDashGravity = false;
	};

	// ダッシュ状態
	enum class State {
		kStart,	// 開始
		kPlay,	// 中
		kEnd,	// 最後
		kNone,	// 無し
	};


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	// 更新
	void Update(float dt, WorldTransform& world, RigidBodyComponent& rigid);
	// ダッシュ開始
	void StartDash();

public:
	// ダッシュ用データの取得
	Data& GetData() { return data_; };
	// 状態取得
	State GetState() const { return state_; }
	// ダッシュしているかどうか取得
	bool IsDash() const { return isDash_; }
	// ダッシュ可能かどうか取得
	bool IsCanDash() const { return canDash_; }
	// 方向設定
	void SetDirection(const Vector3& direction);
	

private:
	// 速度処理
	void SpeedProcess(float dt);
	// 状態処理
	void StateProcess(float dt);
	// ダッシュ処理
	void DashProcess(float dt, WorldTransform& world, RigidBodyComponent& rigid);
	// ダッシュ時の重力処理
	void GravityProcess(float dt, RigidBodyComponent& rigid);
private:
	// ダッシュ用データ
	Data data_{};
	// 状態取得
	State state_{};
	// 速度
	float speed_ = 0.0f;
	// ダッシュ中かどうか
	bool isDash_ = false;
	// ダッシュ可能かどうか
	bool canDash_ = true;

	// 方向
	Vector3 direction_ = {};

private:
	// タイマー
	float timer_ = 0.0f;

};

