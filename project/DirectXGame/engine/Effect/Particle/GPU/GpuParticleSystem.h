#pragma once

#include "GpuParticleData.h"
#include <DirectXGame/engine/Mesh/TrailMesh.h>

class ModelMesh;
class DirectXCommon;
class GpuParticleManager;
class BaseGpuParticleEmitter;
class GpuParticleGroup
{
public:
	void Create(GpuParticleManager* gpuParticleManager, DirectXCommon* dxCommon,int MaxInstance, std::string name, std::string textureName);


	void UpdateEmitte(float deltaTime);

	void UpdateField();

	void Update();

	void Draw();

public: // トレイル用

	void UpateTrailEmitte(float deltaTime);

	void UpdateTrail();

	void DrawTrail();

public:


	void AddEmitter(BaseGpuParticleEmitter* emit);

	void SetMesh(ModelMesh* mesh) { this->mesh_ = mesh; }	// メッシュセット

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }


	std::string GetName() const { return name_; }


	std::string GetTextureName() const { return textureName_; }

	int GetMaxInstance() const {return cbMaxInstance_.Data()->maxInstance;}

	void Reset() { cbDeleteParticleCS_.Data()->isDelete = true; };

private:
	ModelMesh* mesh_ = nullptr;		// モデルメッシュ
	std::string textureName_ = "";	// テクスチャ名前(パーティクル)
	std::string trailTextureName_ = "";	// テクスチャ名前(トレイル)

	std::string name_ = "no";		// 名前
	// CS用のパーティクルデータ
	StructuredBuffer<ParticleCS> sbParticleResource_;
	// CS用のカウントインデックス
	StructuredBuffer<FreeListIndex> sbFreeListIndexResource_;
	// CS用のカウント
	StructuredBuffer<uint32_t> sbFreeListResource_;
	// パーティクル上限
	ConstantBuffer<MaxInstance> cbMaxInstance_;
	// 時間
	ConstantBuffer<PerFrame> cbPerFrame_;
	// パーティクル削除情報
	ConstantBuffer<DeleteParticleCS> cbDeleteParticleCS_;
	//






private:
	// エミッターたち
	std::map<std::string, BaseGpuParticleEmitter*> emitters;

	StructuredBuffer<EmitterCommon> cbEmitterCommon_;	// 共通データ
	StructuredBuffer<EmitterTrail> cbEmitterTrail_;	// トレイルエミッター用データ
	StructuredBuffer<PerEmitterDispatch> emitterDispatchBuffer_;
	ConstantBuffer<DispatchCount> cbDispatchCount_;

	int threadCount = 64;

	int emitCount_ = 0;
private:
	
private: // トレイル用

	// トレイル頂点バッファ
	StructuredBuffer<GpuTrailVertex> sbTrailVertexResource_;
	// トレイル上限
	ConstantBuffer<MaxInstance> cbMaxTrailVertexInstance_;


	// カメラ位置
	ConstantBuffer<Vector3> cbCameraPos_;


	float deleteTimer_ = 0.0f;	// パーティクル削除タイマー

private:
	SrvManager* srvManager_ = nullptr;		// SRVマネージャー
	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
	Camera* camera_ = nullptr;				// カメラ
	GpuParticleManager* gpuParticleManager_;
};

