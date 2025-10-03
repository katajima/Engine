#include "Entity2DManager.h"

void Entity2DManager::Initialize(DirectXCommon* directXCommon)
{
	directXCommon_ = directXCommon;

	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(directXCommon_);

}
