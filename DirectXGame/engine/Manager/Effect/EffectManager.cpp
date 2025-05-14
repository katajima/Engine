#include "EffectManager.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void EffectManager::Initialize(DirectXCommon* directXCommon, LightManager* lightManager, LineCommon* lineCommon)
{
	directXCommon_ = directXCommon;
	lightManager_ = lightManager;
	lineCommon_ = lineCommon;

	trailEffectCommon_ = std::make_unique<TrailEffectCommon>();
	trailEffectCommon_->Initialize(directXCommon_);

	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(directXCommon_, lightManager_,this);

}


