#include "BulletSpawn.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Bullet/Base/BulletManager.h>


#include "DirectXGame/application/base/Bullet/PlayerRangeBombingBullet.h"
#include "DirectXGame/application/base/Bullet/EnemyBullet.h"
#include "DirectXGame/application/base/Bullet/PlayerBullet.h"


void BulletSpawn::Initialize(BaseCharacter* owner, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
	Engine::GlobalVariables* globalVariables, Engine::Camera* camera, Effect* effect) {
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	camera_ = camera;
	effect_ = effect;
	owner_ = owner;
	bulletManager_ = owner_->GetBulletManager();
}



void BulletSpawn::GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad)
{
	std::unique_ptr<BaseBullet> bullet;
	bullet = std::make_unique<PlayerRangeBombingBullet>();
	bullet->SetTargetRange(targetPos, rad);	// 範囲とターゲット設定
	bullet->SetEffect(effect_);	// エフェクトセット
	bullet->SetOwner(owner_);	// 持ち主設定
	bullet->Initialize(entity3DManager_, entity2DManager_, globalVariables_, position, camera_);// 弾の初期化

	bulletManager_->AddBullet(std::move(bullet));
}

void BulletSpawn::GenerateBullet(BulletType type, const BulletInfo& info, BaseCharacter* target)
{
	std::unique_ptr<BaseBullet> bullet;
	if (type == BulletType::kEnemyBullet) {
		bullet = std::make_unique<EnemyBullet>();
		bullet->SetInfo(info); // 設定
		bullet->SetEffect(effect_);	// エフェクトセット
		bullet->SetTarget(target);	// 敵セット
		bullet->SetOwner(owner_);	// 持ち主設定
		bullet->Initialize(entity3DManager_, entity2DManager_, globalVariables_, info.position, camera_); // 弾の初期化
	}
	else {
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		bullet->SetInfo(info); // 設定
		bullet->SetEffect(effect_);	// エフェクトセット
		bullet->SetTarget(target);	// 敵セット
		bullet->SetOwner(owner_);	// 持ち主設定
		bullet->Initialize(entity3DManager_, entity2DManager_, globalVariables_, info.position, camera_); // 弾の初期化
	}
	bulletManager_->AddBullet(std::move(bullet));
}
