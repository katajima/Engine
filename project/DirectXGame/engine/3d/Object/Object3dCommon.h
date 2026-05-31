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
		// 初期化
		void Initialize(DirectXCommon* dxCommon);
		// DirectXの共通クラス
		DirectXCommon* GetDxCommon() const { return dxCommon; }
		// 描画設定
		void DrawCommonSetting(PSOType type) { psoManager_->DrawSetting(type, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); }
		// シャドウマップ描画設定
		void DrawShadowMapSetting() { psoManager_->DrawSetting(PSOType::ShadowMap, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); }
		// デフォルトカメラ設定
		void SetDefaltCamera(Camera* camera) { this->defaultCamera = camera; }
		// デフォルトカメラ取得
		Camera* GetDefaltCamera() const { return defaultCamera; }
		// オブジェクトカウント取得
		int32_t GetObjectCount() const { return objectCount; }
		// オブジェクトカウント加算
		void AddObjectCount() { objectCount++; }
		// オブジェクトカウントクリア
		void ClearObjectCount() { objectCount = 0; }
	private:
		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();
		// シャドウマップ用パイプラインの作成
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
