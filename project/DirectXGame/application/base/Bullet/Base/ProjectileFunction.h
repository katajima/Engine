#pragma once
#include "DirectXGame/engine/Math/MathFunctions.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"

namespace Projectile {

	// ホーミング
	void Homing(float dt,Engine::WorldTransform& world, const Vector3& targetPos, Vector3& dire, float speed, bool enable, float strength, float range);

	// 放物線
	void Parabola(float dt,Engine::WorldTransform& world,Engine::RigidBodyComponent* rigid, Vector3& dire,const Vector3& velo, float speed, float gravityScale);

	// 直線
	void Straight(float dt,Engine::WorldTransform& world,const Vector3& dire, float speed);

	// 維持
	void Stay(float dt,Engine::WorldTransform& world);

}