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
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator);
public:	// ジャンプ開始
	void StartJump(Engine::RigidBodyComponent& rigid);
	/// <summary>
	/// ジャンプ回数現象
	/// </summary>
	void DecrementJumpCount() { jumpCount_--; }
public:
	/// <summary>
	/// ジャンプデータ取得
	/// </summary>
	JumpData GetData() const { return data_; }
	//
	JumpData& Data() { return data_; }
	/// <summary>
	/// ジャンプ出来るか
	/// </summary>
	bool GetIsJump() const { return jumpCount_ > 0; }
	/// <summary>
	/// 残りジャンプ回数取得
	/// </summary>
	int GetJumpCount() const { return jumpCount_; }
	//
	bool GetIsJumping() const { return isJumping_;}
	/// <summary>
	/// データ設定
	/// </summary>
	void SetData(const JumpData& data) { data_ = data; }
	/// <summary>
	/// 最大ジャンプカウント設定
	/// </summary>
	void SetMaxJumpCount(int count) { data_.maxJumpCount = count; }
	/// <summary>
	/// 入力中か設定
	/// </summary>
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
