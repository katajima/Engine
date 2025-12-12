#include "EffectManager.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::EffectManager::Initialize(DirectXCommon* directXCommon, LightManager* lightManager, LineCommon* lineCommon)
{
	directXCommon_ = directXCommon;// DX共通クラス
	lightManager_ = lightManager;  // ライト管理
	lineCommon_ = lineCommon;	   // ライン管理

	// トレイルエフェクト共通クラス初期化
	trailEffectCommon_ = std::make_unique<TrailEffectCommon>();
	trailEffectCommon_->Initialize(directXCommon_);

	// パーティクル管理クラス初期化
	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(directXCommon_, lightManager_, this);

	// GPUパーティクル管理クラス初期化
	gpuParticleManager_ = std::make_unique<GpuParticleManager>();
	gpuParticleManager_->Initialize(directXCommon_, lightManager_, this);

	// パーティクル2d管理クラス初期化
	particleManager2d_ = std::make_unique<ParticleManager2d>();
	particleManager2d_->Initialize(directXCommon_, this);


}




