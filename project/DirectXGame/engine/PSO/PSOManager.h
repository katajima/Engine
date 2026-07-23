#pragma once

#include "psoData.h"
#include "map"


namespace Engine {
	// 前方宣言
	class Command;
	class DXGIDevice;
	class DXCCompiler;

	/// <summary>
	/// PSOマネージャークラス
	/// </summary>
	class PSOManager
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler);

		// ルートパラメータ設定
		void SetRootSignature(
			Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
			D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters,
			D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT numSamplers);


		// グラフィックパイプライン生成
		void GraphicsPipelineState(Microsoft::WRL::ComPtr < ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr < ID3D12PipelineState>& graphicsPipelineState
			, D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE  topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		/// <summary>
		/// Inputエレメントデスク追加
		/// </summary>
		void AddInputElementDesc(const std::string& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT slot = 0);

		/// <summary>
		/// シェーダーファイル設定
		/// </summary>
		void SetShaderFileName(ShaderFileName shaderFileName, std::wstring filename);
		/// <summary>
		/// デプスステンシルデスク設定
		/// </summary>
		void SetDepthStencilDesc(bool flag, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func);
		/// <summary>
		/// ラスタライザ設定
		/// </summary>
		void SetRasterizerDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill);
		/// <summary>
		/// RTV/DSV形式設定
		/// </summary>
		void SetRenderTargetFormats(UINT numRenderTargets, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
		/// <summary>
		/// 描画準備
		/// </summary>
		void DrawSetting(PSOType type, D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// PSO生成
		void CreatePso(PSOType type, D3D12_ROOT_PARAMETER* rootParameter, UINT rootNum, D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT samplerNum,
			D3D12_CULL_MODE cull, D3D12_FILL_MODE fill,
			D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE  topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		/// <summary>
		/// インプットレイアウトを無くす
		/// </summary>
		void NoInputLayout() { useInputLayout_ = false; };

	private:
		Command* command  = nullptr;
		DXGIDevice* dxgiDevice = nullptr;
		DXCCompiler* dxcCompiler = nullptr;

		//
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_{};
		std::vector<std::string> semanticNames_; // SemanticName を保持するための vector
		std::vector<UINT> semanticIndex_;
		std::vector<DXGI_FORMAT> semanticformat_;
		std::vector<UINT> semanticSlot_;

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
		D3D12_RASTERIZER_DESC rasterizerDesc_{};
		UINT numRenderTargets_ = 1;
		DXGI_FORMAT rtvFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		DXGI_FORMAT dsvFormat_ = DXGI_FORMAT_D24_UNORM_S8_UINT;


		ShaderFileSet shaderFiles_; // 全シェーダーステージのファイルパス


		bool useInputLayout_ = true;

	private:
		std::map<PSOType, PSRS> psoRoots_;
	private:
		/// <summary>
		/// グラフィックパイプライン設定
		/// </summary>
		void SetShederGraphics(D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipeline);
	};
}
