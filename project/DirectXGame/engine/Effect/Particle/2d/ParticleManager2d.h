#pragma once
// DirectX
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

// DirectXTex
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/PSO/PSOManager.h"
#include"DirectXGame/engine/Effect/Trail/TrailEffect.h"

#include <DirectXGame/engine/Effect/Particle/CPU/ParticleData.h>
#include "DirectXGame/engine/Utility/MapUtility.h"

class Material;
class SrvManager;
class EffectManager;
class Primitive2D;
class ParticleManager2d
{
public:
	ParticleManager2d() = default;
	~ParticleManager2d() = default;
	ParticleManager2d(ParticleManager2d&) = delete;
	ParticleManager2d& operator=(ParticleManager2d&) = delete;


	// 初期化
	void Initialize(DirectXCommon* dxCommon, EffectManager* effectManager);
	// 更新
	void Update();
	// 描画
	void Draw();




	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }

	// ランダムエンジン
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

	// パーティクルグループ作り(スプライト)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, Sprite* sprite);

	// パーティクルグループ作り(プリミティブ2D)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, Primitive2D* primitive2d);

	// パーティクルグループ作り(プリミティブ2D)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, ModelMesh* modelMesh);


private:
	// 描画準備
	void DrawCommonSetting();

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private:
	DirectXCommon* dxCommon_ = nullptr;		// DirectX
	SrvManager* srvManager_ = nullptr;		// SRV
	EffectManager* effectManager_ = nullptr;	// エフェクト
	Camera* camera_ = nullptr;				// カメラ
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

