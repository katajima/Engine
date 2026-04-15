#include "ProjectileFunction.h"

// ホーミング
void Projectile::Homing(float dt,Engine::WorldTransform& world,const Vector3& targetPos, Vector3& dire,float speed, bool enable, float strength, float range) {

	// =========================
	// 現在の前方向を rotation から算出
	// 座標系 : X=Right, Y=Up, Z=Forward
	// rotation_.x = Pitch
	// rotation_.y = Yaw
	// =========================
	const float pitch = world.rotate_.x;
	const float yaw = world.rotate_.y;

	Vector3 forward = {
		std::sin(yaw) * std::cos(pitch),
		-std::sin(pitch),
		std::cos(yaw) * std::cos(pitch)
	};
	forward = Normalize(forward);

	// デフォルトは直進
	Vector3 moveDir = forward;
	dire = moveDir;
	// =========================
	// ホーミング判定
	// =========================
	Vector3 toTarget = {
		targetPos.x - world.translate_.x,
		targetPos.y - world.translate_.y,
		targetPos.z - world.translate_.z
	};

	const float distSq = toTarget.LengthSq();
	const float rangeSq = range * range;

	// enable が true かつ、range <= 0 なら常時ホーミング、
	// range > 0 なら範囲内のみホーミング
	const bool canHoming =
		enable &&
		(distSq > 0.00001f) &&
		(range <= 0.0f || distSq <= rangeSq);

	if (canHoming) {
		Vector3 targetDir = Normalize(toTarget);

		// strength は「1秒あたりどれくらい向きを寄せるか」のイメージ
		float t = Math::Clamp(strength * dt, 0.0f, 1.0f);

		// 現在の向きからターゲット方向へ徐々に曲げる
		moveDir = Normalize(Lerp(forward, targetDir, t));

		// 向きを移動方向へ合わせる
		world.rotate_.y = std::atan2(moveDir.x, moveDir.z);

		const float horizontalLen = std::sqrt(moveDir.x * moveDir.x + moveDir.z * moveDir.z);
		world.rotate_.x = std::atan2(-moveDir.y, horizontalLen);
	}

	// =========================
	// 移動
	// =========================
	world.translate_.x += moveDir.x * speed * dt;
	world.translate_.y += moveDir.y * speed * dt;
	world.translate_.z += moveDir.z * speed * dt;


}

// 放物線
void Projectile::Parabola(float dt,Engine::WorldTransform& world, Engine::RigidBodyComponent* rigid, Vector3& dire,const Vector3& velo, float speed, float gravityScale) {
	Vector3 velocity = velo;
	Vector3 direction = Normalize(velocity);
	

	rigid->SetGravityScale(gravityScale);
	rigid->Integrate(dt, world);

	direction = Normalize(rigid->GetVelocity());
	dire = direction;

	world.rotate_ = Math::DirectionToRotate(direction,Dire::Z);
	world.Update();
}

// 直線
void Projectile::Straight(float dt,Engine::WorldTransform& world,const Vector3& dire, float speed) {
	world.translate_ += dire * speed * dt;
}

// 維持
void Projectile::Stay(float dt,Engine::WorldTransform& world) {
}
