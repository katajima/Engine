#include "BaseEnemy.h"

void BaseEnemy::OnCollision(Collider* other)
{

}

Vector3 BaseEnemy::GetCenterPosition() const
{
	return object_->GetWorldPosition();
}

void BaseEnemy::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
}
