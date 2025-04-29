#include "EnemyManager.h"

void EnemyManager::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	camera_ = camera;

}

void EnemyManager::Update()
{
	for (auto& enemy : enemys_) {
		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemys_) {
		enemy->Draw();
	}
}

void EnemyManager::Draw2D()
{
	for (auto& enemy : enemys_) {
		enemy->Draw2D();
	}
}

void EnemyManager::GenerateEnemy(EnemyType type, Vector3 position)
{
	std::unique_ptr<BaseEnemy> enemy;
	switch (type)
	{
	case EnemyManager::EnemyType::kNormal:
		enemy = std::make_unique<NormalEnemy>();
		//enemy->;

		
		break;
	case EnemyManager::EnemyType::kBullet:
		break;
	case EnemyManager::EnemyType::kAttacker:
		break;
	case EnemyManager::EnemyType::kTank:
		break;
	case EnemyManager::EnemyType::kElite:
		break;
	case EnemyManager::EnemyType::kBoss:
		break;
	default:
		
		break;
	}


	enemy->Initialize(entity3DManager_, entity2DManager_, position, camera_);
	enemy->SetPlayer(player_);

	enemys_.push_back(std::move(enemy));


}
