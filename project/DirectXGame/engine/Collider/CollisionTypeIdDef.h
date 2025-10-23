#pragma once

#include <cstdint>

// コリジョン種別ID定義
enum class CollisionTag {
	None,
	Player,
	Enemy,
	PlayerAttack,
	EnemyAttack,
	Wall,
	Item,
};

// レイヤー
enum class CollisionLayer : uint8_t {
	Default,
	Player,
	Enemy,
	PlayerAttack,
	EnemyAttack,
	Environment,
};

// コライダタイプ
enum class ColliderType
{
	Sphere,
	Capsule,
	AABB,
	OBB,
	Mesh,		// 追加予定
	Segment,
	Ray,
};
