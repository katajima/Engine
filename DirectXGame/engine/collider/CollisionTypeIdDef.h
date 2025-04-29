#pragma once

#include <cstdint>

// コリジョン種別ID定義
enum class CollisionTypeIdDef : uint32_t {
	kdefault,
	kPlayer,
	kPlayerWeapon,
	kEnemy,
	kEnemyWeapon,
	kEnemyWeapon2,
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
	Box,
	Mesh,
};
