#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/PSO/PSOManager.h"


namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// トレイルエフェクト共通クラス
	/// </summary>
	class TrailEffectCommon
	{
	public:
		// 初期化
		void Initialize(DirectXCommon* dxcommon);
		// 描画準備
		void DrawCommonSetting();
		// DirectX共通クラス取得
		DirectXCommon* GetDxCommon() const { return dxCommon_; }
	private:
		// ルートシグネチャの作成
		void CreateRootSignature();
		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();

	private:
		DirectXCommon* dxCommon_;
		std::unique_ptr<PSOManager> psoManager_ = nullptr;

		//ルートシグネチャデスク
		D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
		//ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;
	};
}
