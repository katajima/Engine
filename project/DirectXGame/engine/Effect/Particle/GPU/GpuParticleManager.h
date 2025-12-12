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
	class Entity3DManager;
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

		// 初期化
		void Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* efectManager);

		// 更新
		void Update();
		// 描画
		void Draw();


		// カメラセット
		void SetCamera(Camera* camera) { this->camera_ = camera; }
		// コンピュートシェーダでのPSO設定
		void PreCsPso();
		// コンピュートシェーダトレイルのPSO設定
		void PreCsTrailPso();
		// グループ生成
		void CreateGroup(std::string name, ModelMesh* mesh, std::string textureName, int instance);



		// エミッターにグループを関連付け
		void SetEmitteToGroup(std::string emitteName, std::string particleGroupName);
		// フィールド生成
		void CreateField(std::string name);

		// エミッターの解放
		void ClearEmitter(std::string nume);
		// 全エミッターの開放
		void ClearEmitterAll();


		// パーティクル削除
		void ClearGroupParticle(std::string name);
		// パーティクル削除(全て)
		void ClearGroupParticleAll();

		// フィールドの解放
		void ClearField(std::string name);

		// ヘッダにテンプレート関数を定義
		template <typename EmitterType>
		void CreateEmitter(const std::string& name) {
			if (gpuParticleEmitter_.contains(name)) {
				return;
			}

			auto emitter = std::make_unique<EmitterType>();
			emitter->Init(dxCommon_, lineCommon_, nullptr, name);
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

		// パーティクルグループ取得
		GpuParticleGroup* GetGpuParticleGroup(const std::string& name) {
			auto it = gpuParticleGroup_.find(name);
			if (it == gpuParticleGroup_.end()) {
				return nullptr;
			}
			return &it->second;
		}

	private:

		// ルートシグネチャの作成
		void CreateRootSignature();
		// グラフィックスパイプラインの作成
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
		Entity3DManager* entity3DManager_;		// エンティティマネージャー
		EffectManager* effectManager_;			// エフェクトマネージャー
		SrvManager* srvManager_ = nullptr;		// SRVマネージャー
		DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
		LineCommon* lineCommon_ = nullptr;		// ライン
		Camera* camera_ = nullptr;				// カメラ
	};
}
