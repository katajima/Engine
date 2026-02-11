#include "BulletSpawn.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Bullet/Base/BulletManager.h>


#include "DirectXGame/application/base/Bullet/PlayerRangeBombingBullet.h"
#include "DirectXGame/application/base/Bullet/EnemyBullet.h"
#include "DirectXGame/application/base/Bullet/PlayerBullet.h"


void BulletSpawn::Initialize(Character::BaseCharacter* owner, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
	Engine::GlobalVariables* globalVariables, Engine::Camera* camera, Effect* effect) {
	this->entity3DManager = entity3DManager;
	this->entity2DManager = entity2DManager;
	this->globalVariables = globalVariables;
	this->camera = camera;
	this->effect = effect;
	this->owner = owner;
	this->bulletManager = owner->GetBulletManager();
}



void BulletSpawn::GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad)
{
	std::unique_ptr<BaseBullet> bullet;
	bullet = std::make_unique<PlayerRangeBombingBullet>();
	bullet->SetTargetRange(targetPos, rad);	// 範囲とターゲット設定
	bullet->SetEffect(effect);	// エフェクトセット
	bullet->SetOwner(owner);	// 持ち主設定
	bullet->Initialize(entity3DManager, entity2DManager, globalVariables, position, camera);// 弾の初期化

	bulletManager->AddBullet(std::move(bullet));
}

void BulletSpawn::GenerateBullet(BulletType type, const BulletInfo& info, Character::BaseCharacter* target)
{
	std::unique_ptr<BaseBullet> bullet;
	if (type == BulletType::kEnemyBullet) {
		bullet = std::make_unique<EnemyBullet>();
		bullet->SetInfo(info); // 設定
		bullet->SetEffect(effect);	// エフェクトセット
		bullet->SetTarget(target);	// 敵セット
		bullet->SetOwner(owner);	// 持ち主設定
		bullet->Initialize(entity3DManager, entity2DManager, globalVariables, info.position, camera); // 弾の初期化
	}
	else {
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		bullet->SetInfo(info); // 設定
		bullet->SetEffect(effect);	// エフェクトセット
		bullet->SetTarget(target);	// 敵セット
		bullet->SetOwner(owner);	// 持ち主設定
		bullet->Initialize(entity3DManager, entity2DManager, globalVariables, info.position, camera); // 弾の初期化
	}
	bulletManager->AddBullet(std::move(bullet));
}
