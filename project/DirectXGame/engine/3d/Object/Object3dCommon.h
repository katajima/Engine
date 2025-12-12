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
		DirectXCommon* GetDxCommon() const { return dxCommon_; }
		// 描画設定
		void DrawCommonSetting(PSOType type) { psoManager_->DrawSetting(type, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); }
		// デフォルトカメラ設定
		void SetDefaltCamera(Camera* camera) { this->defaultCamera = camera; }
		// デフォルトカメラ取得
		Camera* GetDefaltCamera() const { return defaultCamera; }

		int32_t count = 0;
	private:
		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();
	private:// メンバ変数
		DirectXCommon* dxCommon_;

		Camera* defaultCamera = nullptr;

		std::unique_ptr<PSOManager> psoManager_ = nullptr;



	private:// メンバ変数


	};
}
