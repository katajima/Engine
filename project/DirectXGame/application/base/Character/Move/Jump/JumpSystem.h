#pragma once
#include "JumpData.h"
#include "DirectXGame/application/base/Character/Move/Base/LocomotionCoordinator.h"

namespace Engine {
	class RigidBodyComponent;
}

/// <summary>
/// ジャンプシステムクラス
/// </summary>
class JumpSystem {
public:
	// 初期化
	void Initialize();
	// 更新
	void Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator);
public:	// ジャンプ開始
	void StartJump(Engine::RigidBodyComponent& rigid);
	// ジャンプ回数現象
	void DecrementJumpCount() { jumpCount_--; }
public:
	// ジャンプデータ取得
	JumpData GetData() const { return data_; }
	//
	JumpData& Data() { return data_; }
	//	ジャンプ出来るか
	bool GetIsJump() const { return jumpCount_ > 0; }
	// 残りジャンプ回数取得
	int GetJumpCount() const { return jumpCount_; }
	//
	bool GetIsJumping() const { return isJumping_;}
	// データ設定
	void SetData(const JumpData& data) { data_ = data; }
	// 最大ジャンプカウント設定
	void SetMaxJumpCount(int count) { data_.maxJumpCount = count; }
	// 入力中か設定
	void SetInputPressed(bool isPressed) { isInputPressed_ = isPressed; }
private:
	// ジャンプデータ
	JumpData data_{};
	// ジャンプ回数
	int jumpCount_ = 0;
private: // フラグ系統
	// 着地しているかどうか
	bool isLanding_ = false;
	// ジャンプ中かどうか
	bool isJumping_ = false;
	// 入力が押されているかどうか
	bool isInputPressed_ = false;
private:
	// 入力ホールドタイマー
	float holdTimer_ = 0.0f;
};
