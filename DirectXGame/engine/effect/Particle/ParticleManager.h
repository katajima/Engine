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
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include"DirectXGame/engine/Effect/Trail/TrailEffect.h"

#include "ParticleData.h"
#include "DirectXGame/engine/Effect/Particle/Emit/EmitFanction.h"
#include "ParticleField.h"


#include "DirectXGame/engine/Utility/MapUtility.h"

class LightManager;
class Material;
class BasePrimitive;
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
	void DrawCommonSetting(EmitData::RasterizerType rasteType, EmitData::BlendType blendType);

	// パーティクルの発生
	void Emit(const std::string name,WorldTransform& transform, EmitData::EmitType type, EmitData::SpawnType spawnType);

	// パーティクルグループ取得
	UnorderedMapContainer<std::string, ParticleGroup>& GetParticleGroups()
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
		EmitData::RasterizerType rasteType = EmitData::RasterizerType::MODE_SOLID_BACK, EmitData::BlendType blendType = EmitData::BlendType::MODE_ADD);

	// パーティクルグループ作り(プリミティブ)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, BasePrimitive* primitive,
		EmitData::RasterizerType rasteType = EmitData::RasterizerType::MODE_SOLID_BACK, EmitData::BlendType blendType = EmitData::BlendType::MODE_ADD);

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }

	
	void AddFieldEffect(Field::FieldEffect* field) {
		fieldEffect_.push_back(field);
	}
	
private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

	
	void BlendAdd();

	void BlendSubtract();

	void BlendMuliply();

private: // もらいもの
	LightManager* lightManager_ = nullptr;	// ライト
	DirectXCommon* dxCommon_ = nullptr;		// DirectX
	SrvManager* srvManager_ = nullptr;		// SRV
	EffectManager* efectManager_ = nullptr;	// エフェクト
	LineCommon* lineCommon_ = nullptr;		// ライン
	Camera* camera_ = nullptr;				// カメラ
private:
	// PSO設定
	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	// ランダムエンジン
	std::mt19937 randomEngine_;

	UnorderedMapContainer<std::string, ParticleGroup> particleGroups;

	// 最大パーティクル量
	const uint32_t kNumMaxInstance = 1024 * 4;
	
	// フィールド関係
	std::vector<Field::FieldEffect*> fieldEffect_;

private: //PSO関係 
	////ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	////ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	//// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[6];

	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	DebugTimer debugTimer_;
};