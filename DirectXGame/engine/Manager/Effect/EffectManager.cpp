#include "EffectManager.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void EffectManager::Initialize(DirectXCommon* directXCommon)
{
	directXCommon_ = directXCommon;


	trailEffectCommon_ = std::make_unique<TrailEffectCommon>();
	trailEffectCommon_->Initialize(directXCommon_);

	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(directXCommon_);

}


