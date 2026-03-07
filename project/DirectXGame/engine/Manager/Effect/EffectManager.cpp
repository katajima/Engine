#include "EffectManager.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::EffectManager::Initialize(DirectXCommon* directXCommon, LightManager* lightManager, LineCommon* lineCommon)
{
	this->dxCommon = directXCommon;// DX共通クラス
	this->lightManager = lightManager;  // ライト管理
	this->lineCommon = lineCommon;	   // ライン管理

	// トレイルエフェクト共通クラス初期化
	trailEffectCommon_ = std::make_unique<TrailEffectCommon>();
	trailEffectCommon_->Initialize(dxCommon);

	// パーティクル管理クラス初期化
	particleManager_ = std::make_unique<ParticleManager>();
	particleManager_->Initialize(dxCommon, lightManager, this);

	// GPUパーティクル管理クラス初期化
	gpuParticleManager_ = std::make_unique<GpuParticleManager>();
	gpuParticleManager_->Initialize(dxCommon, lightManager, this);

	// パーティクル2d管理クラス初期化
	particleManager2d_ = std::make_unique<ParticleManager2d>();
	particleManager2d_->Initialize(dxCommon, this);


}




