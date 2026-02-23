#include "MovementSystem.h"

void MovementSystem::Initialize()
{
}

void MovementSystem::Update(const LocomotionContext& cxt, const MoveCommand& cmd, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid) {


	world.translate_ += cmd.finalVelocity;

	// 重力処理
	GravityProess(cxt,world,rigid);

	world.Update();

}
void MovementSystem::GravityProess(const LocomotionContext& cxt,Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid){

	if (!useGravity) {
		rigid.SetIsGravity(!useGravity);
		return;
	}


	if (world.GetWorldPosition().y <= groundHeight_) {
		world.translate_.y = groundHeight_;	// 地面位置に
		rigid.Velocity().y = 0.0f;			// y速度を0に
		rigid.SetIsGravity(false);			// 重力をオフ
		rigid.SetGravityScale(1.0f);		// 重力スケールリセット
		onGround_ = true;					// 着地
	}
	else {
		if (!cxt.isAttacking) {
			if (rigid.GetVelocity().y < 0.0f) {
				rigid.SetGravityScale(cxt.fallGravity);	// 重力スケールセット
			}
			else {
				rigid.SetGravityScale(cxt.upGravity);		// 重力スケールセット
			}
		}
		else {
			rigid.SetGravityScale(cxt.attackingGravity);	// 重力スケールセット
		}
		onGround_ = false;			// 着地状態を解除
	}
};
