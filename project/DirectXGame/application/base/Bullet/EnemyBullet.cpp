#include "EnemyBullet.h"

#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"

#include <limits>

void EnemyBullet::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
	EffectSystem* effectSystem, const Projectile::ProjectileSpawnInfo& spawnInfo, const BulletInfo& info) {
	// 旧敵弾と同じ見た目、当たり判定、寿命をBaseProjectileへ設定する
	Projectile::ProjectileParam param{};
	param.name = "enemyBullet";
	param.modelName = "AnimatedCube.gltf";
	param.speed = info.speed;
	param.maxLifeTime = 5.0f;
	param.radius = 0.2f;
	param.damage = info.damage;
	param.type = Projectile::Type::Normal;
	param.moveType = Projectile::ProjectileMoveType::Straight;
	param.hitType = info.type == ProjectileType::PENETRATION ?
		Projectile::ProjectileHitType::Penetrate : Projectile::ProjectileHitType::Destroy;
	param.maxPierceCount = info.type == ProjectileType::PENETRATION ? (std::numeric_limits<int>::max)() : 0;
	param.collisionTag = CollisionTag::EnemyAttack;
	param.collisionLayer = CollisionLayer::EnemyAttack;
	param.collisionMask = static_cast<uint32_t>(CollisionLayer::Player);

	Projectile::ProjectileSpawnInfo resolvedSpawnInfo = spawnInfo;
	resolvedSpawnInfo.scale = Vector3::Set(0.3f);
	Projectile::BaseProjectile::Initialize(entityManager, globalVariables, effectSystem, resolvedSpawnInfo, param);
}

void EnemyBullet::OnHitPlayer(Character::BasePlayer* player) {
	if (!player) {
		return;
	}
	if (player->GetCurrentMainState() == Character::CharacterMainState::Avoidance) {
		player->OnDodgeSuccess();
		return;
	}
	// 旧敵弾と同様にダメージだけを与え、プレイヤー側の状態判断へ任せる
	player->AddDamage(GetParam().damage);
}
