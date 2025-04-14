#include "Entity3DManager.h"

void Entity3DManager::Initialize(DirectXCommon* directXCommon)
{
	directXCommon_ = directXCommon;

	// オーシャンシェーダー
	oceanManager_ = std::make_unique<OceanManager>();
	oceanManager_->Initialize(directXCommon_);


	object3dInstansManager_ = std::make_unique<Object3dInstansManager>();
	object3dInstansManager_->Initialize(directXCommon_);
}
