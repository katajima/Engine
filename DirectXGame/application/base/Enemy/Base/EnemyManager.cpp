#include "EnemyManager.h"

void EnemyManager::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;


}

void EnemyManager::GenerateEnemy(EnemyType type, Vector3 position)
{
}
