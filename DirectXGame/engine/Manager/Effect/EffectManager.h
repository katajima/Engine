#pragma once

#include "memory"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/Effect/Trail/TrailEffectManager.h"
#include "DirectXGame/engine/Effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/Effect/Particle/GPU/GpuParticleManager.h"


class LightManager;
class DirectXCommon;
class LineCommon;
class EffectManager
{
public:
	EffectManager() = default;
	~EffectManager() = default;

	// 初期化
	void Initialize(DirectXCommon* directXCommon, LightManager* lightManager, LineCommon* lineCommon);


public: // ゲッター

	// DirectXCommon
	DirectXCommon* GetDxCommon() { return directXCommon_; }
	// トレイルエフェクト用
	TrailEffectCommon* GetTrailEffectCommon() { return trailEffectCommon_.get(); }
	// パーティクルマネージャ
	ParticleManager* GetParticleManager() { return particleManager_.get(); }
	// パーティクルマネージャ
	GpuParticleManager* GetGpuParticleManager() { return gpuParticleManager_.get(); }


	//
	LineCommon* GetLineCommon() { return lineCommon_; }

private:
	// トレイルエフェクト
	std::unique_ptr<TrailEffectCommon> trailEffectCommon_;

	// パーティクルマネージャー
	std::unique_ptr<ParticleManager> particleManager_;

	// GPUパーティクルマネージャー
	std::unique_ptr<GpuParticleManager> gpuParticleManager_;


private:
	DirectXCommon* directXCommon_;
	LightManager* lightManager_;
	LineCommon* lineCommon_;
};







