#pragma once

#include <cstdint>

// コリジョン種別ID定義
enum class CollisionTag {
	None,			// 何もなし
	Player,			// プレイヤー
	Enemy,			// 敵
	PlayerAttack,	// プレイヤー攻撃
	EnemyAttack,	// 敵攻撃
	Wall,			// 障害物(壁や床)
	Item,			// アイテム
	PlayerEffect,	// プレイヤーによる影響
	EnemyEffect,	// 敵による影響
};

// レイヤー
enum class CollisionLayer : uint32_t {
	None = 0,
	Player = 1 << 0,
	Enemy = 1 << 1,
	PlayerAttack = 1 << 2,
	EnemyAttack = 1 << 3,
	Environment = 1 << 4,

	ALL = 0xFFFFFFFF
};

// コライダタイプ
enum class ColliderShapeType
{
	Sphere,
	Capsule,
	AABB,
	OBB,
	Triangle,
	Mesh,		// 追加予定
	Segment,
	Ray,
};
