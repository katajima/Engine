#pragma once

#include "GpuParticleData.h"

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

	SrvManager* srvManager_ = nullptr;		// SRVマネージャー
	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
	Camera* camera_ = nullptr;				// カメラ
	GpuParticleManager* gpuParticleManager_;
};

