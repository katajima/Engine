#pragma once

#include "GpuParticleData.h"
#include <DirectXGame/engine/Mesh/TrailMesh.h>

class ModelMesh;
class DirectXCommon;
class GpuParticleManager;
class GpuParticleGroup
{
public:
	void Create(GpuParticleManager* gpuParticleManager, DirectXCommon* dxCommon,int MaxInstance, std::string name, std::string textureName);


	void UpdateEmitte(float deltaTime, int count);

	void UpdateField();

	void Update();



public: // トレイル用

	void UpateTrailEmitte(float deltaTime);

	void UpdateTrail();



	void Draw();


	void SetMesh(ModelMesh* mesh) { this->mesh_ = mesh; }	// メッシュセット

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }


	std::string GetName() const { return name_; }

	int GetMaxInstance() const {return cbMaxInstance_.Data()->maxInstance;}

private:
	ModelMesh* mesh_ = nullptr;		// モデルメッシュ
	std::string textureName_ = "";	// テクスチャインデック
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



private: // トレイル用

	// トレイル頂点バッファ
	StructuredBuffer<GpuTrailVertex> sbTrailVertexResource_;
	// CS用のトレイル頂点カウントインデックス
	StructuredBuffer<FreeListIndex> sbTrailVertexFreeListIndexResource_;
	// CS用のカウント
	StructuredBuffer<uint32_t> sbTrailVertexFreeListResource_;
	// トレイル上限
	ConstantBuffer<MaxInstance> cbMaxTrailVertexInstance_;



	std::unique_ptr<TrailMesh> trailMesh_ = nullptr;	// トレイルメッシュ
	


	// カメラ位置
	ConstantBuffer<Vector3> cbCameraPos_;

private:
	SrvManager* srvManager_ = nullptr;		// SRVマネージャー
	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
	Camera* camera_ = nullptr;				// カメラ
	GpuParticleManager* gpuParticleManager_;
};

