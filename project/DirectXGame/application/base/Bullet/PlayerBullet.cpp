#include "PlayerBullet.h"

#include "DirectXGame/application/base/Attack/AttackController.h"
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include <limits>

void PlayerBullet::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
	EffectSystem* effectSystem, const Projectile::ProjectileSpawnInfo& spawnInfo, const BulletInfo& info) {
	// 旧プレイヤー弾と同じ見た目、当たり判定、寿命をBaseProjectileへ設定する
	Projectile::ProjectileParam param{};
	param.name = "playerStanBullet";
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
	param.collisionTag = CollisionTag::PlayerAttack;
	param.collisionLayer = CollisionLayer::PlayerAttack;
	param.collisionMask = static_cast<uint32_t>(CollisionLayer::Enemy);

	Projectile::ProjectileSpawnInfo resolvedSpawnInfo = spawnInfo;
	resolvedSpawnInfo.scale = Vector3::Set(0.2f);
	Projectile::BaseProjectile::Initialize(entityManager, globalVariables, effectSystem, resolvedSpawnInfo, param);
}

void PlayerBullet::OnHitEnemy(Character::BaseEnemy* enemy) {
	if (!enemy) {
		return;
	}
	// 旧プレイヤー弾と同様にダメージだけを与え、敵側の状態判断へ任せる
	enemy->AddDamage(GetParam().damage);

	// 旧プレイヤー弾が行っていたコンボ命中通知を維持する
	Character::BaseCharacter* bulletOwner = GetOwner();
	if (bulletOwner && bulletOwner->GetAttackController()) {
		bulletOwner->GetAttackController()->GetHitCounter().Hit();
		bulletOwner->GetAttackController()->GetComboSystem()->NotifyAttackHit();
	}
	if (enemy->GetCharacterParameterComponent()->GetHP() > 0) {
		enemy->GetHitMotionSystem()->EmitHitEffect();
	}
}
