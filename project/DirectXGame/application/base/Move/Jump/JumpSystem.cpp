#include "JumpSystem.h"


void JumpSystem::Initialize(){}

void JumpSystem::Update(const LocomotionContext& ctx, LocomotionCoordinator& coordinator,Engine::RigidBodyComponent& rigid)
{
	// 入力ホールド処理
	InputHoldProcess(ctx.dt);
	// ジャンプホールド処理
	JumpHoldProcess(ctx.dt,rigid);

	int j = jumpCount_;

	// 着地
	if (ctx.onGround) {
		jumpCount_ = data_.maxJumpCount_;	// ジャンプ回数リセット
	}
}

void JumpSystem::StartJump(Engine::RigidBodyComponent& rigid)
{
	rigid.Velocity().y = 0;	// 移動速度リセット
	DecrementJumpCount();	// ジャンプ回数減少


	isJumping_ = true;		// ジャンプ中
	isInputHeld_ = true;	// 入力ホールド
	isInputPressed_ = true;	// 入力プレス
	holdTimer_ = 0.0f;		// ホールドタイマーリセット

	rigid.AddForce({ 0,data_.power_,0 }); // 上方向に力を加える
}


#pragma region Process

void JumpSystem::InputHoldProcess(float dt)
{
	if (!isInputHeld_) return;

	// ホールド中かつ入力が押されているならホールドタイマーを進める
	if (isInputPressed_) {
		holdTimer_ += dt;
		if (holdTimer_ >= data_.inputDelay_) {
			isInputHeld_ = false;
		}
	}
	else {
		isInputHeld_ = false;
	}
	
}

void JumpSystem::JumpHoldProcess(float dt, Engine::RigidBodyComponent& rigid)
{
	if (!isInputHeld_) return;	// ホールド解除されているなら処理しない

	// ホールド中の処理
	if (isInputHeld_) {
		rigid.AddForce({ 0,data_.power_,0 }); // 上方向に力を加える
	}
}

#pragma endregion // 処理

