#include "EnemyManager.h"

#include "BaseEnemy.h"

#include "Enemy.h"

void EnemyManager::Initialize(Camera* camera, FollowCamera* followCamera, Player* player)
{
	baseEnemy_.clear();

	
	player_ = player;
	camera_ =  camera;
	followCamera_ = followCamera;
}

void EnemyManager::Update()
{
	for (auto& enemy : baseEnemy_) {
		enemy->Update();
	}

}

void EnemyManager::Draw()
{
	for (auto& enemy : baseEnemy_) {
		enemy->Draw();
	}

}

void EnemyManager::AddMoveTarget(float hp, Vector3 pos)
{
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Initialize(pos, hp, camera_);
	enemy->SetFollowCamera(followCamera_);
	enemy->SetPlayer(player_);
	baseEnemy_.push_back(std::move(enemy));
}
