#include "JumpSystem.h"


void JumpSystem::Initialize()
{
	groundHeight_ = -3.0f;
}

void JumpSystem::Update(float dt, WorldTransform& world, RigidBodyComponent& rigid)
{
	// 現在のy速度取得
	velocity_ = rigid.Velocity().y;

	// 入力ホールド処理
	InputHoldProcess(dt);
	// ジャンプホールド処理
	JumpHoldProcess(dt,rigid);

	// 着地
	if (world.GetWorldPosition().y <= groundHeight_) {
		world.translate_.y = groundHeight_;	// 地面位置に
		rigid.Velocity().y = 0.0f;			// y速度を0に
		rigid.SetIsGravity(false);			// 重力をオフ
		rigid.SetGravityScale(1.0f);		// 重力スケールリセット
		jumpCount_ = data_.maxJumpCount_;	// ジャンプ回数リセット
		isLanding_ = true;					// 着地
	}
	else {
		// ダッシュ中には重力の切り替えはダッシュシステムに任せる
		if (!isDash_ || !isAttack_) {
			rigid.SetIsGravity(true);					// 重力オン
		}
		if (!isAttack_) {
			if (state_ == State::Fall) {
				rigid.SetGravityScale(data_.fallGravity_);	// 重力スケールセット
			}
			else {
				rigid.SetGravityScale(data_.upGravity_);	// 重力スケールセット
			}
		}
		isLanding_ = false;							// 着地していない
	}

	/// 状態処理
	StateProcess();
}

void JumpSystem::StartJump(RigidBodyComponent& rigid)
{
	rigid.Velocity().y = 0;	// 移動速度リセット
	DecrementJumpCount();	// ジャンプ回数減少


	isGrounded_ = false;	// 地面に接していない
	isJumping_ = true;		// ジャンプ中
	isInputHeld_ = true;	// 入力ホールド
	isInputPressed_ = true;	// 入力プレス
	holdTimer_ = 0.0f;		// ホールドタイマーリセット

	rigid.AddForce({ 0,data_.power_,0 }); // 上方向に力を加える
}


#pragma region Process

void JumpSystem::StateProcess() {

	if (isLanding_) {
		state_ = State::Land;
	}
	else {
		if(velocity_ > 0.0f) {
			state_ = State::Jump;
		}
		else {
			state_ = State::Fall;
		}
	}
}

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

void JumpSystem::JumpHoldProcess(float dt, RigidBodyComponent& rigid)
{
	if (!isInputHeld_) return;	// ホールド解除されているなら処理しない

	// ホールド中の処理
	if (isInputHeld_) {
		rigid.AddForce({ 0,data_.power_,0 }); // 上方向に力を加える
	}
}

#pragma endregion // 処理

