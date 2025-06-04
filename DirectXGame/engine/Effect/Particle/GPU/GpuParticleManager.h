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
	Vector3 transrate;
	Vector3 scale;
	Vector3 lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
};

struct PreView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
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

	// 球エミッター
	ConstantBuffer<EmitterSphere> cbEmitterSphere_;



	ModelMesh* mesh_ = nullptr;		// モデルメッシュ
	std::string textureName_ = "";	// テクスチャインデック
private:
	// PSO設定
	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	// CS用のPSO設定
	std::unique_ptr<CSPSOManager> csPsoManager_ = nullptr;
	// CS用のPSO設定
	std::unique_ptr<CSPSOManager> csEmitPsoManager_ = nullptr;

	////ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	////ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	//// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;


	//ルートシグネチャコンピュート
	Microsoft::WRL::ComPtr < ID3D12RootSignature> computeRootSignature;
	// コンピュートパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> computePipelineState;


	//ルートシグネチャコンピュート
	Microsoft::WRL::ComPtr < ID3D12RootSignature> computeEmitRootSignature;
	// コンピュートパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> computeEmitPipelineState;



private:
	Entity3DManager* entity3DManager_;		// エンティティマネージャー
	EffectManager* effectManager_;			// エフェクトマネージャー
	SrvManager* srvManager_ = nullptr;		// SRVマネージャー
	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
	Camera* camera_ = nullptr;				// カメラ
};

