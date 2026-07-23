#pragma once

#include"DirectXGame/engine/PSO/PSOManager.h"


namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// スカイボックス共通クラス
	/// </summary>
	class SkyBoxCommon
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
		void DrawCommonSetting();

	private:
		/// <summary>
		/// ルートシグネチャの作成
		/// </summary>
		void CreateRootSignature();
		/// <summary>
		/// グラフィックスパイプラインの作成
		/// </summary>
		void CreateGraphicsPipeline();

	private:// メンバ変数
		//
		std::unique_ptr<PSOManager> psoManager_;
		//ルートシグネチャデスク
		D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
		//ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;
	private:
		DirectXCommon* dxCommon = nullptr;
	};
}
