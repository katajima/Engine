#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"Collider.h"


namespace CollFanction
{
	// 球と球
	void SphereAndSphere(Collider* colliderA, Collider* colliderB);

	// カプセルとカプセル
	void CapsuleAndCapsule(Collider* colliderA, Collider* colliderB);

	// AABBとAABB 
	void AABBAndAABB(Collider* colliderA, Collider* colliderB);

}



