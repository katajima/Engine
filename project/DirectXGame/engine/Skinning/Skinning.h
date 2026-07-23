#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/PSO/CSPSOManager.h"

namespace Engine {
	/// <summary>
	/// スキニング共通クラス
	/// </summary>
	class SkinningCommon
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		/// <summary>
		/// DirectX共通クラス取得
		/// </summary>
		DirectXCommon* GetDxCommon() const { return dxCommon; }

		/// <summary>
		/// 描画前準備
		/// </summary>
		void DrawCommonSetting(PSOType type) { psoManager_->DrawSetting(type); }
		/// <summary>
		/// シャドウマップ描画前準備
		/// </summary>
		void DrawShadowMapSetting() { psoManager_->DrawSetting(PSOType::ShadowMap); }

		/// <summary>
		/// 計算前準備
		/// </summary>
		void DrawComputeSetting() { csPsoManager_->PreComputePSRS(); }
		/// <summary>
		/// デフォルトカメラ設定
		/// </summary>
		void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
		/// <summary>
		/// デフォルトカメラ取得
		/// </summary>
		Camera* GetDefaultCamera() const {
			defaultCamera;
			return defaultCamera;
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
		/// <summary>
		/// シャドウマップ用パイプラインの作成
		/// </summary>
		void CreateShadowMapPipeline();


	private:// メンバ変数
		DirectXCommon* dxCommon = nullptr;

		std::unique_ptr<PSOManager> psoManager_ = nullptr;

		std::unique_ptr<CSPSOManager> csPsoManager_ = nullptr;


		Camera* defaultCamera = nullptr;
	};
}
