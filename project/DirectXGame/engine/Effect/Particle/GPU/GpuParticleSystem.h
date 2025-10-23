#pragma once

#include "GpuParticleData.h"
#include <DirectXGame/engine/Mesh/TrailMesh.h>

// 前方宣言
class ModelMesh;
class DirectXCommon;
class GpuParticleManager;
class BaseGpuParticleEmitter;

/// <summary>
/// GPUパーティクルグループクラス
/// </summary>
class GpuParticleGroup
{
public:
	// 生成
	void Create(GpuParticleManager* gpuParticleManager, DirectXCommon* dxCommon,int MaxInstance, std::string name, std::string textureName);

	// エミッター更新
	void UpdateEmitte(float deltaTime);
	// フィールド更新
	void UpdateField();
	// 更新
	void Update();
	// 描画
	void Draw();

public: // トレイル用
	// トレイルエミッター更新
	void UpateTrailEmitte(float deltaTime);
	// トレイル更新
	void UpdateTrail();
	// トレイル描画
	void DrawTrail();

public:

	// エミッター追加
	void AddEmitter(BaseGpuParticleEmitter* emit);
	// メッシュ設定
	void SetMesh(ModelMesh* mesh) { this->mesh_ = mesh; }	// メッシュセット

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }

	// 名前で取得
	std::string GetName() const { return name_; }

	// テクスチャ名取得
	std::string GetTextureName() const { return textureName_; }
	// 最大使用量取得
	int GetMaxInstance() const {return cbMaxInstance_.Data()->maxInstance;}
	// リセット
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

