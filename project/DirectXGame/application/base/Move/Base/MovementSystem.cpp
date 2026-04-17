#include "MovementSystem.h"

void MovementSystem::Initialize()
{
}

void MovementSystem::Update(const Character::CharacterContext& cxt, const MoveCommand& cmd, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid) {


	world.translate_ += cmd.finalVelocity;
	if (cmd.finalDirection.Length() != 0.0f) {
		direction_ = cmd.finalDirection;
	}
	else {
		direction_ = world.GetForward();
	}
	//isLinding_ = cmd.isLanding;
	groundHeight_ = cmd.groundHeight;
	// 回転処理
	RotateProcess(cxt, world);

	world.Update();
	// 重力処理
	GravityProess(cxt, world, rigid);

	world.Update();
}


void MovementSystem::GravityProess(const Character::CharacterContext& cxt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid) {

	if (!useGravity) {
		rigid.SetIsGravity(!useGravity);
		return;
	}

	float currentY = world.GetWorldPosition().y;
	float velocityY = rigid.GetVelocity().y * cxt.dt;

	if (currentY + velocityY <= groundHeight_) {
		isLinding_ = true;
	}
	else {
		isLinding_ = false;
	}

	if (isLinding_) {
		world.translate_.y = groundHeight_;
		rigid.ResetAcceleration();			// 加速度リセット
		rigid.ResetVelocity();				// 速度リセット
		rigid.SetIsGravity(false);			// 重力をオフ
		rigid.SetGravityScale(1.0f);		// 重力スケールリセット
	}
	else {
		rigid.SetIsGravity(cxt.isGravity);
		if (!cxt.isGravity) {
			rigid.ResetAcceleration();			// 加速度リセット
			rigid.ResetVelocity();				// 速度リセット
		}

		// 重力スケールセット
		if (!cxt.isAttacking) {
			if (cxt.isDamage) {
				rigid.SetGravityScale(cxt.damageGravity);
			}
			else {
				if (rigid.GetVelocity().y < 0.0f) {
					rigid.SetGravityScale(cxt.fallGravity);
				}
				else {
					rigid.SetGravityScale(cxt.upGravity);
				}
			}
		}
		else {
			rigid.SetGravityScale(cxt.attackingGravity);
		}
	}

	// 重力
	rigid.Integrate(cxt.dt, world);
}

void MovementSystem::RotateProcess(const Character::CharacterContext& cxt, Engine::WorldTransform& world) {
	// 移動ベクトルがゼロなら回転処理しない
	if (direction_.Length() == 0.0f) return;

	// 目標方向（X=Right, Y=Up, Z=Forward）
	float targetYaw = std::atan2(direction_.x, direction_.z);

	float& currentYaw = world.rotate_.y;

	// ----------------------------
	// 角度差を [-π, π] に正規化
	// ----------------------------
	float delta = targetYaw - currentYaw;
	while (delta > DirectX::XM_PI)  delta -= DirectX::XM_2PI;
	while (delta < -DirectX::XM_PI)  delta += DirectX::XM_2PI;

	// ----------------------------
	// 補間（rotationSpeed は 0〜1 の割合）
	// rotationSpeed = 0.1f なら 10% だけ近づく
	// ----------------------------
	currentYaw += delta * rotationSpeed;
}
