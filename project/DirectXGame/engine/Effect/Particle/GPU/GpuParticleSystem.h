#pragma once

#include "GpuParticleData.h"
#include <DirectXGame/engine/Mesh/TrailMesh.h>


namespace Engine {
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
		/// <summary>
		/// 生成
		/// </summary>
		void Create(GpuParticleManager* gpuParticleManager, DirectXCommon* dxCommon, int MaxInstance, std::string name, std::string textureName);

		/// <summary>
		/// エミッター更新
		/// </summary>
		void UpdateEmitte(float deltaTime);
		/// <summary>
		/// フィールド更新
		/// </summary>
		void UpdateField();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

	public: // トレイル用
		/// <summary>
		/// トレイルエミッター更新
		/// </summary>
		void UpateTrailEmitte(float deltaTime);
		/// <summary>
		/// トレイル更新
		/// </summary>
		void UpdateTrail();
		/// <summary>
		/// トレイル描画
		/// </summary>
		void DrawTrail();

	public:

		/// <summary>
		/// エミッター追加
		/// </summary>
		void AddEmitter(BaseGpuParticleEmitter* emit);
		/// <summary>
		/// メッシュ設定
		/// </summary>
		void SetMesh(ModelMesh* mesh) { this->mesh = mesh; }	// メッシュセット

		/// <summary>
		/// カメラセット
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; }

		/// <summary>
		/// 名前で取得
		/// </summary>
		std::string GetName() const { return name_; }

		/// <summary>
		/// テクスチャ名取得
		/// </summary>
		std::string GetTextureName() const { return textureName_; }
		/// <summary>
		/// 最大使用量取得
		/// </summary>
		int GetMaxInstance() const { return cbMaxInstance_.Data()->maxInstance; }
		/// <summary>
		/// リセット
		/// </summary>
		void Reset() { cbDeleteParticleCS_.Data()->isDelete = true; };

	private:
		ModelMesh* mesh = nullptr;		// モデルメッシュ
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
		std::map<std::string, Engine::BaseGpuParticleEmitter*> emitters;

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
		SrvManager* srvManager = nullptr;		// SRVマネージャー
		DirectXCommon* dxCommon = nullptr;		// DirectX共通クラス
		Camera* camera = nullptr;				// カメラ
		GpuParticleManager* gpuParticleManager = nullptr;
	};
}
