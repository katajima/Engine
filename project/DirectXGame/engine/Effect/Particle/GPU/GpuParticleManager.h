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


namespace Engine {
	// 前方宣言
	class LineCommon;
	class LightManager;
	class EntityManager;
	class EffectManager;
	class SrvManager;
	class DirectXCommon;
	class ModelMesh;
	class Camera;

	/// <summary>
	/// GPUパーティクルマネージャー
	/// </summary>
	class GpuParticleManager {
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* efectManager);

		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();


		/// <summary>
		/// カメラセット
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; }
		/// <summary>
		/// コンピュートシェーダでのPSO設定
		/// </summary>
		void PreCsPso();
		/// <summary>
		/// コンピュートシェーダトレイルのPSO設定
		/// </summary>
		void PreCsTrailPso();
		/// <summary>
		/// グループ生成
		/// </summary>
		void CreateGroup(std::string name, ModelMesh* mesh, std::string textureName, int instance);



		/// <summary>
		/// エミッターにグループを関連付け
		/// </summary>
		void SetEmitteToGroup(std::string emitteName, std::string particleGroupName);
		/// <summary>
		/// フィールド生成
		/// </summary>
		void CreateField(std::string name);

		/// <summary>
		/// エミッターの解放
		/// </summary>
		void ClearEmitter(std::string nume);
		/// <summary>
		/// 全エミッターの開放
		/// </summary>
		void ClearEmitterAll();


		/// <summary>
		/// パーティクル削除
		/// </summary>
		void ClearGroupParticle(std::string name);
		/// <summary>
		/// パーティクル削除(全て)
		/// </summary>
		void ClearGroupParticleAll();

		/// <summary>
		/// フィールドの解放
		/// </summary>
		void ClearField(std::string name);

		// ヘッダにテンプレート関数を定義
		template <typename EmitterType>
		void CreateEmitter(const std::string& name) {
			if (gpuParticleEmitter_.contains(name)) {
				return;
			}

			auto emitter = std::make_unique<EmitterType>();
			emitter->Init(dxCommon, lineCommon, nullptr, name);
			gpuParticleEmitter_[name] = std::move(emitter);
		}

		// パーティクルエミッターの取得
		template <typename T>
		T* GetGpuParticleEmitter(const std::string& name) {
			auto it = gpuParticleEmitter_.find(name);
			if (it == gpuParticleEmitter_.end()) {
				return nullptr;
			}
			return dynamic_cast<T*>(it->second.get());
		}

		/// <summary>
		/// パーティクルグループ取得
		/// </summary>
		GpuParticleGroup* GetGpuParticleGroup(const std::string& name) {
			auto it = gpuParticleGroup_.find(name);
			if (it == gpuParticleGroup_.end()) {
				return nullptr;
			}
			return &it->second;
		}

	private:

		/// <summary>
		/// ルートシグネチャの作成
		/// </summary>
		void CreateRootSignature();
		/// <summary>
		/// グラフィックスパイプラインの作成
		/// </summary>
		void CreateGraphicsPipeline();

	private:
		// ビュー情報
		ConstantBuffer<PreView> cbPreViewResource_;


		std::map<std::string, GpuParticleGroup> gpuParticleGroup_;


		std::map<std::string, std::unique_ptr<BaseGpuParticleEmitter>> gpuParticleEmitter_;


		std::map<std::string, GpuParticleField> gpuParticleField_;

	private:
		// PSO設定
		std::unique_ptr<PSOManager> psoManager_ = nullptr;
		// CS用のPSO設定(初期化)
		std::unique_ptr<CSPSOManager> csPsoManager_ = nullptr;
		// CS用のPSO設定(エミッターAABB)
		std::map<EmitterType, std::unique_ptr<CSPSOManager>> csEmitPsoManagers_;
		// CS用のPSO設定(更新)
		std::unique_ptr<CSPSOManager> csUpdatePsoManager_ = nullptr;
		// CS用のPSO設定(影響場所)
		std::unique_ptr<CSPSOManager> csFieldPsoManager_ = nullptr;
		// パーティクル描画PSO
		PSRS particleDraw;



	private:
		// PSO設定(トレイルエフェクト)
		std::unique_ptr<PSOManager> psoTrailManager_ = nullptr;
		// CS用のPSO設定(トレイル初期化)
		std::unique_ptr<CSPSOManager> csTrailInitPsoManager_ = nullptr;
		// CS用のPSO設定(トレイルエミッター)
		std::unique_ptr<CSPSOManager> csTrailEmitPsoManager_ = nullptr;
		// CS用のPSO設定(トレイル更新)
		std::unique_ptr<CSPSOManager> csTrailUpdatePsoManager_ = nullptr;
		// トレイル描画PSO
		PSRS trailDraw;


	private:
		EntityManager* entity3DManager = nullptr;		// エンティティマネージャー
		EffectManager* effectManager = nullptr;			// エフェクトマネージャー
		SrvManager* srvManager = nullptr;		// SRVマネージャー
		DirectXCommon* dxCommon = nullptr;		// DirectX共通クラス
		LineCommon* lineCommon = nullptr;		// ライン
		Camera* camera = nullptr;				// カメラ
	};
}
