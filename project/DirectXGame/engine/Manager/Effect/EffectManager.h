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
#include "DirectXGame/engine/Effect/Particle/CPU/ParticleManager.h"
#include "DirectXGame/engine/Effect/Particle/GPU/GpuParticleManager.h"
#include <DirectXGame/engine/Effect/Particle/2d/ParticleManager2d.h>

// 前方宣言
namespace Engine {
	class LightManager;
	class DirectXCommon;
	class LineCommon;

	/// <summary>
	/// エフェクトマネージャークラス
	/// </summary>
	class EffectManager
	{
	public:
		EffectManager() = default;
		~EffectManager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* directXCommon, LightManager* lightManager, LineCommon* lineCommon);


	public: // ゲッター

		/// <summary>
		/// DirectXCommon取得
		/// </summary>
		DirectXCommon* GetDxCommon() { return dxCommon; }
		/// <summary>
		/// トレイルエフェクト用取得
		/// </summary>
		TrailEffectCommon* GetTrailEffectCommon() { return trailEffectCommon_.get(); }
		/// <summary>
		/// パーティクルマネージャ取得
		/// </summary>
		ParticleManager* GetParticleManager() { return particleManager_.get(); }
		/// <summary>
		/// パーティクルマネージャ取得
		/// </summary>
		GpuParticleManager* GetGpuParticleManager() { return gpuParticleManager_.get(); }
		/// <summary>
		/// パーティクル2dマネージャー取得
		/// </summary>
		ParticleManager2d* GetParticleManager2d() { return particleManager2d_.get(); }

		/// <summary>
		/// ライン共通クラス取得
		/// </summary>
		LineCommon* GetLineCommon() { return lineCommon; }

	private:
		// トレイルエフェクト
		std::unique_ptr<TrailEffectCommon> trailEffectCommon_;

		// パーティクルマネージャー
		std::unique_ptr<ParticleManager> particleManager_;

		// GPUパーティクルマネージャー
		std::unique_ptr<GpuParticleManager> gpuParticleManager_;

		std::unique_ptr<ParticleManager2d> particleManager2d_;
	private:
		DirectXCommon* dxCommon = nullptr;
		LightManager* lightManager = nullptr;
		LineCommon* lineCommon = nullptr;
	};
}






