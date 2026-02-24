#include "MovementSystem.h"

void MovementSystem::Initialize()
{
}

void MovementSystem::Update(const LocomotionContext& cxt, const MoveCommand& cmd, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid) {


	world.translate_ += cmd.finalVelocity;

	// 方向処理
	DirectionProcess(cxt);

	// 回転処理
	RotateProcess(cxt,world);

	// 重力処理
	GravityProess(cxt,world,rigid);

	world.Update();
}


void MovementSystem::GravityProess(const LocomotionContext& cxt,Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid){

	if (!useGravity) {
		rigid.SetIsGravity(!useGravity);
		return;
	}

	// 重力
	rigid.Integrate(cxt.dt, world);

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
}

void MovementSystem::RotateProcess(const LocomotionContext& cxt, Engine::WorldTransform& world) {
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

void MovementSystem::DirectionProcess(const LocomotionContext& cxt) {
	if (!cxt.camera) {
		return;
	}
	
	Vector2 velo = cxt.input.GetData().moveShick;


	if (velo.Length() == 0.0f) return;
	Matrix4x4 cameraWorldMatrix = Inverse(cxt.camera->GetViewMatrix());

	// カメラの向きに基づいて移動方向をワールド座標系に変換
	Vector3 worldDirection = {
		velo.x * cameraWorldMatrix.m[0][0] + velo.y * cameraWorldMatrix.m[2][0],
		0.0f,
		velo.x * cameraWorldMatrix.m[0][2] + velo.y * cameraWorldMatrix.m[2][2]
	};

	direction_ = Normalize(worldDirection);
}