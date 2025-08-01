#pragma once
// DirectX
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

// DirectXTex
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

// engine
#include "DirectXGame/engine/PSO/CSPSOManager.h"
#include "DirectXGame/engine/PSO/PSOManager.h"


#include "GpuParticleData.h"
#include "GpuParticleSystem.h"
#include "GPUParticleEmitter.h"
#include "GpuParticleField.h"

class LineCommon;
class LightManager;
class Entity3DManager;
class EffectManager;
class SrvManager;
class DirectXCommon;
class ModelMesh;
class Camera;
class GpuParticleManager {
public:

	// 初期化
	void Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* efectManager);

	// 更新
	void Update();
	// 描画
	void Draw();


	void Emit();

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }

	void PreCsPso();


	void CreateGroup(std::string name, ModelMesh* mesh, std::string textureName, int instance);
		
	void CreateEmitter(std::string name);

	void SetEmitteToGroup(std::string emitteName, std::string particleGroupName);

	void CreateField(std::string name);

	GpuParticleEmitter& GetGpuParticleEmitter(std::string name);

private:

	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private:
	// ビュー情報
	ConstantBuffer<PreView> cbPreViewResource_;
	

	std::map<std::string, GpuParticleGroup> gpuParticleGroup_;
	std::map<std::string, GpuParticleEmitter> gpuParticleEmitter_;
	std::map<std::string, GpuParticleField> gpuParticleField_;

private:
	// PSO設定
	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	// CS用のPSO設定(初期化)
	std::unique_ptr<CSPSOManager> csPsoManager_ = nullptr;
	// CS用のPSO設定(エミッター)
	std::unique_ptr<CSPSOManager> csEmitPsoManager_ = nullptr;
	// CS用のPSO設定(更新)
	std::unique_ptr<CSPSOManager> csUpdatePsoManager_ = nullptr;
	// CS用のPSO設定(影響場所)
	std::unique_ptr<CSPSOManager> csFieldPsoManager_ = nullptr;

	
	// パーティクル描画PSO
	PSOManager::PSRS particleDraw;

	// パーティクル初期化PSO
	CSPSOManager::PSRS particleCsInit;
	
	// パーティクルエミッターPSO
	CSPSOManager::PSRS particleCsEmit;

	// パーティクル更新PSO
	CSPSOManager::PSRS particleCsUpdate;

private:
	Entity3DManager* entity3DManager_;		// エンティティマネージャー
	EffectManager* effectManager_;			// エフェクトマネージャー
	SrvManager* srvManager_ = nullptr;		// SRVマネージャー
	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
	LineCommon* lineCommon_ = nullptr;		// ライン
	Camera* camera_ = nullptr;				// カメラ
};

