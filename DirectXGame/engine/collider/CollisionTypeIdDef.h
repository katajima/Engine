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
	// 必要に応じて拡張
};

// 当たる対象選択
struct IsCollisionType 
{
	int isDefault;
	int isPlayer;
	int isPlayerWeapon;
	int isEnemy;
	int isEnemyWeapon;
	int isEnemyWeapon2;
};


enum class ColliderType
{
	Sphere,
	Capsule,
	AABB,
	OBB,
	Mesh,
};
