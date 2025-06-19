#include "BulletManager.h"

#include "DirectXGame/application/base/Player/Bullet/PlayerMissile.h"
#include "DirectXGame/application/base/Player/Bullet/PlayerRangeBombingBullet.h"

void BulletManager::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	camera_ = camera;

}

void BulletManager::Update()
{
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
	bullets_.remove_if([](const std::unique_ptr<BaseBullet>& bullet) { return !bullet->GetAlive() && !bullet->GetIsEffectPlay(); });
}

void BulletManager::Draw()
{
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void BulletManager::DrawEffect()
{
	for (auto& bullet : bullets_) {
		bullet->DrawP();
	}
}

void BulletManager::Draw2D()
{
	for (auto& bullet : bullets_) {
		bullet->Draw2D();
	}
}

void BulletManager::GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad)
{
	std::unique_ptr<BaseBullet> bullet;


	switch (type)
	{
	case BulletManager::BulletType::kPlayerMissile:
		bullet = std::make_unique<PlayerMissile>();
		//bullet->SetTargetType(CollisionTypeIdDef::kEnemy);
		break;
	case BulletManager::BulletType::kEnemyBullet:
		break;
	case BulletManager::BulletType::kRangeBombingSpecial:
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		//bullet->SetTargetType(CollisionTypeIdDef::kEnemy);
		break;
	default:
		break;
	}

	bullet->SetTargerRange(targetPos, rad);
	bullet->Initialize(entity3DManager_, entity2DManager_, position, camera_);
	bullet->SetPlayer(player_);
	

	bullets_.push_back(std::move(bullet));
}

void BulletManager::GenerateBullet(BulletType type, Vector3 position, BaseEnemy* enemy)
{
	std::unique_ptr<BaseBullet> bullet;


	switch (type)
	{
	case BulletManager::BulletType::kPlayerMissile:
		bullet = std::make_unique<PlayerMissile>();
		//bullet->SetTargetType(CollisionTypeIdDef::kEnemy);
		break;
	case BulletManager::BulletType::kEnemyBullet:
		break;
	case BulletManager::BulletType::kRangeBombingSpecial:
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		//bullet->SetTargetType(CollisionTypeIdDef::kEnemy);
		break;
	default:
		break;
	}

	bullet->Initialize(entity3DManager_, entity2DManager_, position, camera_);
	bullet->SetEnemy(enemy);
	bullet->SetPlayer(player_);


	bullets_.push_back(std::move(bullet));

}
