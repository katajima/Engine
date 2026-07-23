#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/PSO/PSOManager.h"


namespace Engine {
	/// <summary>
	/// 3dオブジェクトの共通クラス
	/// </summary>
	class Object3dCommon
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		/// <summary>
		/// DirectXの共通クラス
		/// </summary>
		DirectXCommon* GetDxCommon() const { return dxCommon; }
		/// <summary>
		/// 描画設定
		/// </summary>
		void DrawCommonSetting(PSOType type) { psoManager_->DrawSetting(type, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); }
		/// <summary>
		/// シャドウマップ描画設定
		/// </summary>
		void DrawShadowMapSetting() { psoManager_->DrawSetting(PSOType::ShadowMap, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); }
		/// <summary>
		/// デフォルトカメラ設定
		/// </summary>
		void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
		/// <summary>
		/// デフォルトカメラ取得
		/// </summary>
		Camera* GetDefaultCamera() const { return defaultCamera; }
		/// <summary>
		/// オブジェクトカウント取得
		/// </summary>
		int32_t GetObjectCount() const { return objectCount; }
		/// <summary>
		/// オブジェクトカウント加算
		/// </summary>
		void AddObjectCount() { objectCount++; }
		/// <summary>
		/// オブジェクトカウントクリア
		/// </summary>
		void ClearObjectCount() { objectCount = 0; }
	private:
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
		// カメラ
		Camera* defaultCamera = nullptr;
	private:// メンバ変数
		std::unique_ptr<PSOManager> psoManager_ = nullptr;
		// オブジェクトカウント用
		int32_t objectCount = 0;
	};
}
