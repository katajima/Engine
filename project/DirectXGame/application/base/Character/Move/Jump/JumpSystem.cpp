#include "JumpSystem.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"

void JumpSystem::Initialize(){}

void JumpSystem::Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {
	// 着地
	if (ctx.onGround) {
		jumpCount_ = data_.maxJumpCount;	// ジャンプ回数リセット
	}
}

void JumpSystem::StartJump(Engine::RigidBodyComponent& rigid) {
	DecrementJumpCount();	// ジャンプ回数減少
	isJumping_ = true;		// ジャンプ中
	isInputPressed_ = true;	// 入力プレス
	holdTimer_ = 0.0f;		// ホールドタイマーリセット
	rigid.Velocity().y = data_.power;
}


