#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DashData.h"

/// <summary>
/// ダッシュに関するシステムクラス
/// </summary>
class DashSystem {
public:
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
	void Update(float dt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid);
	// ダッシュ開始
	void StartDash();

public:
	// ダッシュ用データの取得
	DashData& GetData() { return data_; };
	// 状態取得
	State GetState() const { return state_; }
	// ダッシュしているかどうか取得
	bool IsDash() const { return isDash_; }
	// ダッシュ可能かどうか取得
	bool IsCanDash() const { return canDash_; }
	// データ設定
	void SetData(const DashData& data) { data_ = data; }
	// 方向設定
	void SetDirection(const Vector3& direction);
	//
	void SetIsUseGravity(bool isUse) { isUseGravity_ = isUse; }

private:
	// 速度処理
	void SpeedProcess(float dt);
	// 状態処理
	void StateProcess(float dt);
	// ダッシュ処理
	void DashProcess(float dt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid);
	// ダッシュ時の重力処理
	void GravityProcess(float dt, Engine::RigidBodyComponent& rigid);
private:
	// ダッシュ用データ
	DashData data_{};
	// 状態取得
	State state_{};
	// 速度
	float speed_ = 0.0f;
	// ダッシュ中かどうか
	bool isDash_ = false;
	// ダッシュ可能かどうか
	bool canDash_ = true;
	//
	bool isUseGravity_ = false;

	// 方向
	Vector3 direction_ = {};

private:
	// タイマー
	float timer_ = 0.0f;

};

