#pragma once

// DirectX
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

// DirectXTex
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

// engine
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/PSO/PSOManager.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include"DirectXGame/engine/Effect/Trail/TrailEffect.h"

#include "ParticleData.h"
#include "DirectXGame/engine/Effect/Particle/Emit/EmitFunction.h"
#include "ParticleField.h"


#include "DirectXGame/engine/Utility/MapUtility.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"


namespace Engine {
	// 前方宣言
	class LightManager;
	class Material;
	class DirectXCommon;
	class SrvManager;
	class EffectManager;
	class LineCommon;
	class PrimitiveCommon;

	// エディタから復元・編集するためのパーティクル群メタデータ
	struct ParticleGroupEditorData {
		std::string texturePath = "resources/Texture/Image.dds";										// 使用するテクスチャ
		ShapeParameter::ShapeType shapeType = ShapeParameter::ShapeType::Plane;						// エディタ作成時のプリミティブ形状
		ShapeParameter::ShapePlane plane{};															// 平面パラメータ
		ShapeParameter::ShapeTriangle triangle{};														// 三角形パラメータ
		ShapeParameter::ShapeCross cross{};															// 十字パラメータ
		ShapeParameter::ShapeCube cube{};															// 立方体パラメータ
		ShapeParameter::Circle circle{};																// 円パラメータ
		ShapeParameter::Star star{};																	// 星パラメータ
		ShapeParameter::Crescent crescent{};															// 三日月パラメータ
		ShapeParameter::Ring ring{};																	// リングパラメータ
		ShapeParameter::ShapeSphere sphere{};															// 球パラメータ
		ShapeParameter::ShapeArrow arrow{};															// 矢印パラメータ
		ShapeParameter::Cylinder cylinder{};															// 円柱パラメータ
		ShapeParameter::Tube tube{};																	// 筒パラメータ
		ShapeParameter::Pyramid pyramid{};															// 角錐パラメータ
		ShapeParameter::Torus torus{};																// トーラスパラメータ
		EmitData::RasterizerType rasterizerType = EmitData::RasterizerType::MODE_SOLID_BACK;			// カリング設定
		EmitData::BlendType blendType = EmitData::BlendType::MODE_ADD;								// ブレンド設定
		bool isEditorPrimitive = false;																// エディタ所有プリミティブで作ったか
		bool isUVClamp = false;																		// UVクランプ描画を使うか
		Transform uvTransformVelocity{ {}, {}, {0.0f, 0.0f, 0.0f} };									// UVトランスフォームの速度
		EmitData::IsFlag isFlag{};																	// パーティクル挙動フラグ
		EmitData::EmitType emitType = EmitData::EmitType::kRandom;									// メッシュからの発生方法
		EmitData::TopBottom topBottom = EmitData::TopBottom::kBottom;								// ライフタイムスケールの基準
		float gravitationalAcceleration = 9.8f;														// 重力加速度
		Transform materialTransform{};																// マテリアルUVトランスフォーム
		Vector4 materialColor = { 1.0f, 1.0f, 1.0f, 1.0f };											// マテリアルカラー
		bool materialEnableLighting = true;															// ライティングを使うか
		float materialEnvironmentCoefficient = 0.5f;													// 環境マップ係数
		float materialShininess = 20.0f;																// スペキュラの強さ
		bool materialUseLig = false;																	// ライト計算を使うか
		bool materialUseNormalMap = false;															// ノーマルマップを使うか
		bool materialUseSpecularMap = false;															// スペキュラマップを使うか
		bool materialUseEnvironment = false;															// 環境マップを使うか
		float materialAlphaClipping = 0.5f;															// アルファクリッピング値
		float materialAlpha = 1.0f;																	// 全体アルファ
	};

	/// <summary>
	/// パーティクル3dマネージャー
	/// </summary>
	class ParticleManager
	{
	public:
		ParticleManager() = default;
		~ParticleManager() = default;
		ParticleManager(ParticleManager&) = delete;
		ParticleManager& operator=(ParticleManager&) = delete;

		// 初期化
		void Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* efectManager);
		// エディタ作成パーティクルのプリミティブを生成する共通クラスを設定
		void SetPrimitiveCommon(PrimitiveCommon* primitiveCommon) { this->primitiveCommon = primitiveCommon; }
		// 更新
		void Update();
		// 描画
		void Draw();

		// 描画準備
		void DrawCommonSetting(EmitData::RasterizerType rasteType, EmitData::BlendType blendType, bool uvClamp);

		// パーティクルグループ取得
		UnorderedMapContainer<std::string, ParticleGroup>& GetParticleGroups()
		{
			return particleGroups;
		}
		// パーティクルグループ取得
		ParticleGroup& GetParticleGroups(const std::string name)
		{
			return particleGroups[name];
		}
		// ライン共通クラス取得
		LineCommon* GetLineCommon() { return lineCommon; }


		// パーティクルグループ作り(モデル)
		void CreateParticleGroup(const std::string name, const std::string textureFilePath, Model* model,
			EmitData::RasterizerType rasteType = EmitData::RasterizerType::MODE_SOLID_BACK, EmitData::BlendType blendType = EmitData::BlendType::MODE_ADD);

		// パーティクルグループ作り(プリミティブ)
		void CreateParticleGroup(const std::string name, const std::string textureFilePath, BasePrimitive* primitive,
			EmitData::RasterizerType rasteType = EmitData::RasterizerType::MODE_SOLID_BACK, EmitData::BlendType blendType = EmitData::BlendType::MODE_ADD);

		// エディタ用プリミティブを所有してパーティクルグループを作成
		bool CreateEditorParticleGroup(const std::string& name, const ParticleGroupEditorData& data);
		// エディタ用メタデータに合わせて既存パーティクルグループを作り直す
		bool RecreateEditorParticleGroup(const std::string& name, const ParticleGroupEditorData& data);
		// パーティクルグループを削除
		bool RemoveParticleGroup(const std::string& name);
		// パーティクルグループ名を変更
		bool RenameParticleGroup(const std::string& oldName, const std::string& newName);
		// エディタ用メタデータ取得
		ParticleGroupEditorData GetEditorParticleGroupData(const std::string& name) const;
		// エディタ用メタデータ設定
		void SetEditorParticleGroupData(const std::string& name, const ParticleGroupEditorData& data);
		// エディタ用保存データを実際のパーティクル群へ反映
		void ApplyEditorParticleGroupData(const std::string& name, const ParticleGroupEditorData& data);

		// カメラセット
		void SetCamera(Camera* camera) { this->camera = camera; }

		// フィールドエフェクト追加
		void AddFieldEffect(Field::FieldEffect* field) {
			fieldEffect_.push_back(field);
		}

		// ランダムエンジン取得
		std::mt19937& GetRandomEngine() { return randomEngine_; }

		// パーティクルクリア(名前で検索)
		void ClearParticle(std::string name);

		// パーティクルクリア(全て)
		void ClearParticle() {
			for (auto& group : particleGroups) {
				group.second.particle.clear();
			}
		}

	private:
		// ルートシグネチャの作成
		void CreateRootSignature();
		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();

		// ブレンド設定(加算)
		void BlendAdd();
		// ブレンド設定(減算)
		void BlendSubtract();
		// ブレンド設定(乗算)
		void BlendMuliply();

	private: // もらいもの
		LightManager* lightManager = nullptr;	// ライト
		DirectXCommon* dxCommon = nullptr;		// DirectX
		SrvManager* srvManager = nullptr;		// SRV
		EffectManager* efectManager = nullptr;	// エフェクト
		LineCommon* lineCommon = nullptr;		// ライン
		PrimitiveCommon* primitiveCommon = nullptr; // エディタ用プリミティブ生成
		Camera* camera = nullptr;				// カメラ
	private:
		// PSO設定
		std::unique_ptr<PSOManager> psoManager_ = nullptr;

		// ランダムエンジン
		std::mt19937 randomEngine_;

		UnorderedMapContainer<std::string, ParticleGroup> particleGroups;
		// エディタ作成プリミティブの寿命をパーティクルグループと合わせて保持
		std::map<std::string, std::unique_ptr<BasePrimitive>> editorParticlePrimitives_;
		// エディタで保存・復元するためのパーティクル群メタデータ
		std::map<std::string, ParticleGroupEditorData> editorParticleGroupDatas_;

		// 最大パーティクル量
		const uint32_t kNumMaxInstance = 1024 * 2;

		// フィールド関係
		std::vector<Field::FieldEffect*> fieldEffect_;

	private: //PSO関係
		////ルートシグネチャデスク
		D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
		////ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature2;

		//// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[12];

		D3D12_BLEND_DESC blendDesc{};
		D3D12_RASTERIZER_DESC rasterizerDesc{};

		DebugTimer debugTimer_;
	};
}
