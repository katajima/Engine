#include "BulletManager.h"

#include "DirectXGame/application/base/Player/Bullet/PlayerMissile.h"

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
	bullets_.remove_if([](const std::unique_ptr<BaseBullet>& bullet) { return !bullet->GetAlive(); });
}

void BulletManager::Draw()
{
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void BulletManager::Draw2D()
{
	for (auto& bullet : bullets_) {
		bullet->Draw2D();
	}
}

void BulletManager::GeneratBullet(BulletType type, Vector3 position, BaseEnemy* enemy)
{
	std::unique_ptr<BaseBullet> bullet;


	switch (type)
	{
	case BulletManager::BulletType::kPlayerMissile:
		bullet = std::make_unique<PlayerMissile>();
		bullet->SetTargetType(CollisionTypeIdDef::kEnemy);
		break;
	case BulletManager::BulletType::kEnemyBullet:
		break;
	default:
		break;
	}

	bullet->Initialize(entity3DManager_, entity2DManager_, position, camera_);
	bullet->SetEnemy(enemy);
	bullet->SetPlayer(player_);


	bullets_.push_back(std::move(bullet));

}
