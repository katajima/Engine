#include "JumpSystem.h"


void JumpSystem::Initialize()
{
	data_.power_ = 0.0f;
	groundHeight_ = -3.0f;
	data_.inputDelay_ = 0.0f;
}

void JumpSystem::Update(float dt, WorldTransform& world, RigidBodyComponent& rigid)
{
	// 着地
	if (world.GetWorldPosition().y <= groundHeight_) {
		world.translate_.y = groundHeight_;	// 地面位置に
		rigid.Velocity().y = 0.0f;			// y速度を0に
		rigid.SetIsGravity(false);			// 重力をオフ
		rigid.SetGravityScale(1.0f);		// 重力スケールリセット
		data_.jumpCount_ = data_.maxJumpCount_;	// ジャンプ回数リセット
		isLanding_ = true;					// 着地
	}
	else {
		rigid.SetIsGravity(true);			// 重力オン
		rigid.SetGravityScale(data_.gravity_);	// 重力スケールセット
		isLanding_ = false;					// 着地していない
	}
}
