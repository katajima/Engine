#pragma once

#include"DirectXGame/engine/PSO/PSOManager.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
using namespace Microsoft::WRL;



namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// スプライト共通クラス
	/// </summary>
	class SpriteCommon
	{
	public:// メンバ関数
		// 初期化
		void Initialize(DirectXCommon* dxCommon);

		// DxCommon取得
		DirectXCommon* GetDxCommon() const { return dxCommon_; }
		// 描画のうつり方設定
		void DrawCommonSetting(PSOType type) { psoManager_->DrawSetting(type); }

	private:// メンバ関数

		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();

	private:// メンバ変数
		Engine::DirectXCommon* dxCommon_;

		std::unique_ptr<PSOManager> psoManager_ = nullptr;

		//ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature[2];
		// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[4];
	};
}
