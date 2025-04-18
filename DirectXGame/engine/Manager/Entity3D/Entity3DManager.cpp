#include "Entity3DManager.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"


void Entity3DManager::Initialize(DirectXCommon* directXCommon)
{
	directXCommon_ = directXCommon;

	cameraCommon_ = std::make_unique<CameraCommon>();
	cameraCommon_->Initialize(directXCommon_);

	// オーシャンシェーダー
	oceanManager_ = std::make_unique<OceanManager>();
	oceanManager_->Initialize(directXCommon_);


	object3dInstansManager_ = std::make_unique<Object3dInstansManager>();
	object3dInstansManager_->Initialize(directXCommon_);


	object3dCommon_ = std::make_unique<Object3dCommon>();
	object3dCommon_->Initialize(directXCommon_);

	// スカイボックス
	skyBoxCommon_ = std::make_unique<SkyBoxCommon>();
	skyBoxCommon_->Initialize(directXCommon_);

	// ライト
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize(directXCommon_);

	// スキニング
	skinningCommon_ = std::make_unique<SkinningConmmon>();
	skinningCommon_->Initialize(directXCommon_);

	lineCommon_ = std::make_unique<LineCommon>();
	lineCommon_->Initialize(directXCommon_);

	// プリミティブ
	primitiveCommon_ = std::make_unique<PrimitiveCommon>();
	primitiveCommon_->Initialize(directXCommon_);


	// エフェクトマネージャー
	effectManager_ = std::make_unique<EffectManager>();
	effectManager_->Initialize(directXCommon_);
}
