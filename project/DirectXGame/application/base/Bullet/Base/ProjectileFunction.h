#pragma once
#include "DirectXGame/engine/Math/MathFunctions.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"

namespace Projectile {

	/// <summary>
	/// ホーミング
	/// </summary>
	void Homing(float dt,Engine::WorldTransform& world, const Vector3& targetPos, Vector3& dire, float speed, bool enable, float strength, float range);

	/// <summary>
	/// 放物線
	/// </summary>
	void Parabola(float dt,Engine::WorldTransform& world,Engine::RigidBodyComponent* rigid, Vector3& dire,const Vector3& velo, float speed, float gravityScale);

	/// <summary>
	/// 直線
	/// </summary>
	void Straight(float dt,Engine::WorldTransform& world,const Vector3& dire, float speed);

	/// <summary>
	/// 維持
	/// </summary>
	void Stay(float dt,Engine::WorldTransform& world);

}