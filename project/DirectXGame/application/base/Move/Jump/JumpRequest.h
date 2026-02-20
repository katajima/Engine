#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "JumpData.h"

/// <summary>
/// ジャンプシステムクラス
/// </summary>
class JumpRequest {
public:
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
	void Update(float dt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid);
public:	// ジャンプ開始
	void StartJump(Engine::RigidBodyComponent& rigid);
	// ジャンプ回数現象
	void DecrementJumpCount() { jumpCount_--; }
public:
	// ジャンプデータ取得
	JumpData& GetData() { return data_; }
	// ジャンプ状態取得
	State GetState() const { return state_; }
	//	ジャンプ出来るか
	bool GetIsJump() const { return jumpCount_ > 0; }
	// 着地状態か
	bool GetIsLanding() const { return isLanding_; }
	// データ設定
	void SetData(const JumpData& data) { data_ = data; }
	// 最大ジャンプカウント設定
	void SetMaxJumpCount(int count) { data_.maxJumpCount_ = count; }
	// 入力中か設定
	void SetInputPressed(bool isPressed) { isInputPressed_ = isPressed; }
	// ダッシュ中か設定
	void SetIsDash(bool dash) { isDash_ = dash; }
	// 攻撃中か設定
	void SetIsAttack(bool is) { isAttack_ = is; };
	// ジャンプの処理を使うか設定
	void SetIsUseJump(bool isUse) { isUseJump = isUse; }
private:

	// 状態処理
	void StateProcess();

	// 入力処理
	void InputHoldProcess(float dt);

	// ジャンプホールド処理
	void JumpHoldProcess(float dt, Engine::RigidBodyComponent& rigid);

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
	JumpData data_{};
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

	//
	bool isUseJump = true;
private:
	// 入力ホールドタイマー
	float holdTimer_ = 0.0f;
};