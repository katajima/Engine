#pragma once
// DirectX
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

// DirectXTex
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

// engine
#include "DirectXGame/engine/Math/MathFanctions.h"
#include "DirectXGame/engine/DirectX/Resource/StructuredBuffer.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


#include "DirectXGame/engine/PSO/CSPSOManager.h"
#include "DirectXGame/engine/PSO/PSOManager.h"

struct ParticleCS {
	Vector4 color;
	Vector3 transrate;
	float lifeTime;
	Vector3 scale;
	float currentTime;
	Vector3 velocity;
	float pad[1];
};

struct PreView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
};

struct PerFrame
{
	// ゲームを起動してからの時間
	float time;
	// 1フレームの経過時間
	float deltaTime;
};

struct EmitterSphere
{
	Vector3 translate;      // 位置
	float radius;           // 射出半径
	uint32_t count;         // 射出数
	float frequency;        // 射出間隔
	float frequencyTime;    // 射出間隔調整用時間
	uint32_t emit;          // 射出許可
};

struct EffectFieldCS {
	Vector3 translate;      // 位置
	Vector3 range;			// 各半径
	float force;			// 力
	uint32_t isEffect;		// 影響を出すか
};

struct MaxInstance
{
	uint32_t maxInstance;	// 最大個数
};

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

	void SetMesh(ModelMesh* mesh) { this->mesh_ = mesh; }	// メッシュセット

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }


private:

	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private:
	// VS用のビューデータ
	Microsoft::WRL::ComPtr < ID3D12Resource> preViewResource_;
	PreView* preView_;

	// CS用のパーティクルデータ
	StructuredBuffer<ParticleCS> sbParticleResource_;
	// CS用のカウントインデックス
	StructuredBuffer<int32_t> sbFreeListIndexResource_;
	// CS用のカウント
	StructuredBuffer<uint32_t> sbFreeListResource_;
	// 球エミッター
	ConstantBuffer<EmitterSphere> cbEmitterSphere_;
	// 時間
	ConstantBuffer<PerFrame> cbPerFrame_;
	// パーティクル上限
	ConstantBuffer<MaxInstance> cbMaxInstance_;
	// パーティクル影響場所
	ConstantBuffer<EffectFieldCS> cbEffectFieldResource_;

	ModelMesh* mesh_ = nullptr;		// モデルメッシュ
	std::string textureName_ = "";	// テクスチャインデック
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

	////ルートシグネチャデスク
	//D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};

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
	Camera* camera_ = nullptr;				// カメラ
};

