#include "BulletSpawn.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Bullet/Base/BulletManager.h>


#include "DirectXGame/application/base/Bullet/PlayerRangeBombingBullet.h"
#include "DirectXGame/application/base/Bullet/EnemyBullet.h"
#include "DirectXGame/application/base/Bullet/PlayerBullet.h"


void BulletSpawn::Initialize(Character::BaseCharacter* owner, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Engine::Camera* camera, EffectSystem* effect, BulletManager* bulletManager) {
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->camera = camera;
	this->effect = effect;
	this->owner = owner;
	if (owner)
		this->bulletManager = owner->GetBulletManager();
	if (bulletManager)
		this->bulletManager = bulletManager;
}



void BulletSpawn::GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad) {
	std::unique_ptr<BaseBullet> bullet;
	bullet = std::make_unique<PlayerRangeBombingBullet>();
	bullet->SetTargetRange(targetPos, rad);	// 範囲とターゲット設定
	bullet->SetEffect(effect);	// エフェクトセット
	bullet->SetOwner(owner);	// 持ち主設定
	bullet->Initialize(entityManager, globalVariables, position, camera);// 弾の初期化

	bulletManager->AddBullet(std::move(bullet));
}

void BulletSpawn::GenerateBullet(BulletType type, const BulletInfo& info, Character::BaseCharacter* target) {
	std::unique_ptr<BaseBullet> bullet;
	if (type == BulletType::kEnemyBullet) {
		bullet = std::make_unique<EnemyBullet>();
		bullet->SetInfo(info); // 設定
		bullet->SetEffect(effect);	// エフェクトセット
		bullet->SetTarget(target);	// 敵セット
		bullet->SetOwner(owner);	// 持ち主設定
		bullet->Initialize(entityManager, globalVariables, info.position, camera); // 弾の初期化
	}
	else {
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		bullet->SetInfo(info); // 設定
		bullet->SetEffect(effect);	// エフェクトセット
		bullet->SetTarget(target);	// 敵セット
		bullet->SetOwner(owner);	// 持ち主設定
		bullet->Initialize(entityManager, globalVariables, info.position, camera); // 弾の初期化
	}
	bulletManager->AddBullet(std::move(bullet));
}

void BulletSpawn::GenerateProjectile(const Projectile::ProjectileSpawnInfo& spawnInfo,
	const Projectile::ProjectileParam& param, Character::BaseCharacter* target) {
	std::unique_ptr<Projectile::BaseProjectile> projectile = std::make_unique<Projectile::BaseProjectile>();
	// 初期化
	projectile->Initialize(entityManager, globalVariables, effect, spawnInfo, param); // 初期化
	// 追加
	bulletManager->AddProjectile(std::move(projectile));
}
