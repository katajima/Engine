#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"

/// <summary>
/// ジャンプシステムクラス
/// </summary>
class JumpSystem {
public:
	/// <summary>
	/// ジャンプに関連するデータを格納する構造体。
	/// </summary>
	struct Data {
		// ジャンプ力
		float power_ = 800.0f;
		// 上昇時の重力係数
		float upGravity_ = 15.0f;
		// 落下時の重力係数
		float fallGravity_ = 30.0f;
		// 入力受付時間
		float inputDelay_ = 0.1f;
		// ジャンプ入力受付可能かどうか
		bool canInput_ = true;
		// 最大ジャンプ回数
		int maxJumpCount_ = 2;
	};

	// ジャンプ状態列挙型
	enum class State {
		Wait,	// 待機
		Jump,	// ジャンプ
		Fall,	// 落下
		Land,	// 着地
	};


	// 初期化
	void Initialize();

	// 更新
	void Update(float dt,WorldTransform& world, RigidBodyComponent& rigid);


public:	// ジャンプ開始
	void StartJump(RigidBodyComponent& rigid);
	// ジャンプ回数現象
	void DecrementJumpCount() { jumpCount_--; }
public:
	// ジャンプデータ取得
	Data& GetData() { return data_; }
	// ジャンプ状態取得
	State GetState() const { return state_; }
	// ジャンプ可能か設定
	void SetCanInput(bool canInput) { data_.canInput_ = canInput; }
	//	ジャンプ出来るか
	bool GetIsJump() const { return jumpCount_ > 0; }
	// 着地状態か
	bool GetIsLanding() const { return isLanding_; }
	// 最大ジャンプカウント設定
	void SetMaxJumpCount(int count) { data_.maxJumpCount_ = count; }
	// 入力中か設定
	void SetInputPressed(bool isPressed) { isInputPressed_ = isPressed; }
	// ダッシュ中か設定
	void SetIsDash(bool dash) { isDash_ = dash; }
	// 攻撃中か設定
	void SetIsAttack(bool is) { isAttack_ = is; };

private:

	// 状態処理
	void StateProcess();

	// 入力処理
	void InputHoldProcess(float dt);

	// ジャンプホールド処理
	void JumpHoldProcess(float dt, RigidBodyComponent& rigid);

	//


private: // 貰うもの
	// ダッシュ中かどうか
	bool isDash_ = false;
	// 攻撃中かどうか
	bool isAttack_ = false;
private:
	// 地面の高さ
	float groundHeight_ = 0.0f;
	// ジャンプ状態
	State state_ = State::Wait;
private:
	// ジャンプデータ
	Data data_{};
	// ジャンプ回数
	int jumpCount_ = 0;
	// 現在の高さ
	float height_ = 0.0f;
	// 速度
	float velocity_ = 0.0f;
private: // フラグ系統
	// 着地しているかどうか
	bool isLanding_ = false;
	// ジャンプ中かどうか
	bool isJumping_ = false;
	// 地面かどうか
	bool isGrounded_ = false;
	// 入力ホールド中かどうか
	bool isInputHeld_ = false;
	// 入力が押されているかどうか
	bool isInputPressed_ = false;

private:
	// 入力ホールドタイマー
	float holdTimer_ = 0.0f;
};