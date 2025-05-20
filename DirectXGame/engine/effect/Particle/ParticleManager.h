#pragma once

// DirectX
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

// DirectXTex
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

// C++
#include <windows.h>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
using namespace Microsoft::WRL;
#include<memory>
#include<vector>
#include<random>
#include<numbers>



// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/PSO/PSOManager.h"
#include"DirectXGame/engine/WorldTransform/WorldTransform.h"
#include"DirectXGame/engine/Effect/Trail/TrailEffect.h"

#include "ParticleData.h"

class LightManager;
class Material;
class Primitive;
class DirectXCommon;
class SrvManager;
class EffectManager;
class LineCommon;
class ParticleManager
{
public:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;

	// 初期化
	void Initialize(DirectXCommon* dxCommon, LightManager* lightManager,EffectManager* efectManager);
	// 更新
	void Update();
	// 描画
	void Draw();

	// 描画準備
	void DrawCommonSetting(ParticleData::RasterizerType rasteType, ParticleData::BlendType blendType);

	// パーティクルの発生
	void Emit(const std::string name, ParticleData::EmitType type, ParticleData::SpawnType spawnType);

	// パーティクルグループ取得
	std::unordered_map<std::string, ParticleGroup>& GetParticleGroups()
	{
		return particleGroups;
	}
	// パーティクルグループ取得
	ParticleGroup& GetParticleGroups(const std::string name)
	{
		return particleGroups[name];
	}

	LineCommon* GetLineCommon() {return lineCommon_;}


	// パーティクルグループ作り(モデル)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, Model* model,
		ParticleData::RasterizerType rasteType = ParticleData::RasterizerType::MODE_SOLID_BACK, ParticleData::BlendType blendType = ParticleData::BlendType::MODE_ADD);

	// パーティクルグループ作り(プリミティブ)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, Primitive* primitive, 
		ParticleData::RasterizerType rasteType = ParticleData::RasterizerType::MODE_SOLID_BACK, ParticleData::BlendType blendType = ParticleData::BlendType::MODE_ADD);

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

	// ランダム
	void RandParticle(const std::string name, ParticleData::SpawnType spawnType);

	

	void BlendAdd();

	void BlendSubtract();

	void BlendMuliply();

private: // エミッタ種類
	void PointEmit(ParticleGroup& particleGroup); // Point

	void AABBEmit(ParticleGroup& particleGroup); // AABB

	void LineEmit(ParticleGroup& particleGroup); // ライン

	void CornerLineEmit(ParticleGroup& particleGroup); // 角線

	void SplineEmit(ParticleGroup& particleGroup); // スプライン曲線

	void SphereEmit(ParticleGroup& particleGroup); // 球状

private:

	LightManager* lightManager_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	EffectManager* efectManager_ = nullptr;
	LineCommon* lineCommon_ = nullptr;

	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	std::mt19937 randomEngine_;


	std::unordered_map<std::string, ParticleGroup> particleGroups;


	const uint32_t kNumMaxInstance = 12000;
	//float kDeltaTime;
	bool usebillboard = true;
	bool upData = true;
	bool upDataWind = false;
	uint32_t numInstance{};


	Camera* camera_ = nullptr;


	Transform transform{};


	const float kGravitationalAcceleration = 9.8f;
	////ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	////ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	//// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[6];



	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};
};



