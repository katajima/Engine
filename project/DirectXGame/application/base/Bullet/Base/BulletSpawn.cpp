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
	// 旧BulletInfoを共通のProjectile生成情報へ変換する
	Projectile::ProjectileSpawnInfo spawnInfo{};
	spawnInfo.position = info.position;
	spawnInfo.direction = (info.targetPos - info.position).Normalize();
	spawnInfo.owner = owner;
	spawnInfo.target = target;

	if (type == BulletType::kEnemyBullet) {
		auto projectile = std::make_unique<EnemyBullet>();
		projectile->Initialize(entityManager, globalVariables, effect, spawnInfo, info);
		bulletManager->AddProjectile(std::move(projectile));
	}
	else if (type == BulletType::kPlayerBullet) {
		auto projectile = std::make_unique<PlayerBullet>();
		projectile->Initialize(entityManager, globalVariables, effect, spawnInfo, info);
		bulletManager->AddProjectile(std::move(projectile));
	}
	else {
		std::unique_ptr<BaseBullet> bullet = std::make_unique<PlayerRangeBombingBullet>();
		bullet->SetInfo(info); // 設定
		bullet->SetEffect(effect);	// エフェクトセット
		bullet->SetTarget(target);	// 敵セット
		bullet->SetOwner(owner);	// 持ち主設定
		bullet->Initialize(entityManager, globalVariables, info.position, camera); // 弾の初期化
		bulletManager->AddBullet(std::move(bullet));
	}
}

void BulletSpawn::GenerateProjectile(const Projectile::ProjectileSpawnInfo& spawnInfo,
	const Projectile::ProjectileParam& param, Character::BaseCharacter* target) {
	std::unique_ptr<Projectile::BaseProjectile> projectile = std::make_unique<Projectile::BaseProjectile>();
	Projectile::ProjectileSpawnInfo resolvedSpawnInfo = spawnInfo;
	// 呼び出し側でtargetだけ差し替えたい場合に、生成情報へ反映してから渡す。
	if (target) {
		resolvedSpawnInfo.target = target;
	}
	// 初期化
	projectile->Initialize(entityManager, globalVariables, effect, resolvedSpawnInfo, param); // 初期化
	// 追加
	bulletManager->AddProjectile(std::move(projectile));
}

bool BulletSpawn::GenerateProjectile(const Projectile::ProjectileSpawnInfo& spawnInfo,
	const std::string& definitionName, Character::BaseCharacter* target) {
	if (!bulletManager) {
		return false;
	}
	// ProjectileEditorで保存した定義名から実際のProjectileParamを取得する。
	const Projectile::ProjectileParam* param = bulletManager->FindProjectileParam(definitionName);
	if (!param) {
		return false;
	}
	GenerateProjectile(spawnInfo, *param, target);
	return true;
}
