#include "CollisionFancion.h"

void CollFanction::SphereAndSphere(Collider* colliderA, Collider* colliderB)
{
	// コライダーAの座標を取得
	Vector3 posA = colliderA->GetCenterPosition();
	// コライダーBの座標を取得
	Vector3 posB = colliderB->GetCenterPosition();
	// 座標の差分ベクトル
	Vector3 subtract = Subtract(posB, posA);
	// 座標AとBの距離を求める
	float distance = Length(subtract);
	if (distance <= colliderA->GetRadius() + colliderB->GetRadius()) {
		// コライダーAの衝突判時コールバックを呼び出す
		colliderA->OnCollision(colliderB);
		// コライダーBの衝突判時コールバックを呼び出す
		colliderB->OnCollision(colliderA);
	}
}

void CollFanction::CapsuleAndCapsule(Collider* colliderA, Collider* colliderB)
{
	// コライダーAの座標を取得
	Vector3 posA = colliderA->GetCenterPosition();
	// コライダーBの座標を取得
	Vector3 posB = colliderB->GetCenterPosition();
	
	Capsule collA = colliderA->GetCapsule();
	collA.segment.origin += posA;
	collA.segment.end += posA;
	Capsule collB = colliderB->GetCapsule();
	collB.segment.origin += posB;
	collB.segment.end += posB;

	if (IsCollision(collA, collB)) {
		// コライダーAの衝突判時コールバックを呼び出す
		colliderA->OnCollision(colliderB);
		// コライダーBの衝突判時コールバックを呼び出す
		colliderB->OnCollision(colliderA);
	}
}

void CollFanction::AABBAndAABB(Collider* colliderA, Collider* colliderB)
{
	// コライダーAの座標を取得
	Vector3 posA = colliderA->GetCenterPosition();
	// コライダーBの座標を取得
	Vector3 posB = colliderB->GetCenterPosition();

	AABB aabbA = colliderA->GetAABB();
	aabbA.min_ += posA;
	aabbA.max_ += posA;

	AABB aabbB = colliderB->GetAABB();
	aabbB.min_ += posB;
	aabbB.max_ += posB;

	if (IsCollision(aabbA, aabbB)) {
		// コライダーAの衝突判時コールバックを呼び出す
		colliderA->OnCollision(colliderB);
		// コライダーBの衝突判時コールバックを呼び出す
		colliderB->OnCollision(colliderA);
	}
}
