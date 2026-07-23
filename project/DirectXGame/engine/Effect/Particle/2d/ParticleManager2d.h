#pragma once
// DirectX
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

// DirectXTex
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

// engine
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/PSO/PSOManager.h"
#include"DirectXGame/engine/Effect/Trail/TrailEffect.h"

#include <DirectXGame/engine/Effect/Particle/CPU/ParticleData.h>
#include "DirectXGame/engine/Utility/MapUtility.h"


namespace Engine {
	// 前方宣言
	class Material;
	class SrvManager;
	class EffectManager;
	class Primitive2D;

	/// <summary>
	/// パーティクル2dマネージャー
	/// </summary>
	class ParticleManager2d
	{
	public:
		ParticleManager2d() = default;
		~ParticleManager2d() = default;
		ParticleManager2d(ParticleManager2d&) = delete;
		ParticleManager2d& operator=(ParticleManager2d&) = delete;


		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon, EffectManager* effectManager);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();




		/// <summary>
		/// カメラセット
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; }

		/// <summary>
		/// ランダムエンジン
		/// </summary>
		std::mt19937& GetRandomEngine() { return randomEngine_; }

		// パーティクルグループ取得
		UnorderedMapContainer<std::string, ParticleGroup2d>& GetParticleGroups()
		{
			return particleGroups;
		}
		// パーティクルグループ取得
		ParticleGroup2d& GetParticleGroups(const std::string name)
		{
			return particleGroups[name];
		}

		/// <summary>
		/// パーティクルグループ作り(スプライト)
		/// </summary>
		void CreateParticleGroup(const std::string name, const std::string textureFilePath, Sprite* sprite);

		/// <summary>
		/// パーティクルグループ作り(プリミティブ2D)
		/// </summary>
		void CreateParticleGroup(const std::string name, const std::string textureFilePath, Primitive2D* primitive2d);

		/// <summary>
		/// パーティクルグループ作り(プリミティブ2D)
		/// </summary>
		void CreateParticleGroup(const std::string name, const std::string textureFilePath, ModelMesh* modelMesh);


	private:
		/// <summary>
		/// 描画準備
		/// </summary>
		void DrawCommonSetting();

	private:
		/// <summary>
		/// ルートシグネチャの作成
		/// </summary>
		void CreateRootSignature();
		/// <summary>
		/// グラフィックスパイプラインの作成
		/// </summary>
		void CreateGraphicsPipeline();

	private:
		DirectXCommon* dxCommon = nullptr;		// DirectX
		SrvManager* srvManager = nullptr;		// SRV
		EffectManager* effectManager = nullptr;	// エフェクト
		Camera* camera = nullptr;				// カメラ
	private:
		// PSO設定
		std::unique_ptr<PSOManager> psoManager_ = nullptr;

		// ランダムエンジン
		std::mt19937 randomEngine_;

		UnorderedMapContainer<std::string, ParticleGroup2d> particleGroups;

		// 最大パーティクル量
		const uint32_t kNumMaxInstance = 1024 * 4;
	private:
		////ルートシグネチャデスク
		D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
		////ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		//// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;
		D3D12_BLEND_DESC blendDesc{};
		D3D12_RASTERIZER_DESC rasterizerDesc{};

	};
}
