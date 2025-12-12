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
		// 初期化
		void Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler);

		// ルートパラメータ設定
		void SetRootSignature(
			Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
			D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters,
			D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT numSamplers);


		// グラフィックパイプライン生成
		void GraphicsPipelineState(Microsoft::WRL::ComPtr < ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr < ID3D12PipelineState>& graphicsPipelineState
			, D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE  topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		// Inputエレメントデスク追加
		void AddInputElementDesc(const std::string& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT slot = 0);

		// シェーダーファイル設定
		void SetShaderFileName(ShaderFileName shaderFileName, std::wstring filename);
		// デプスステンシルデスク設定
		void SetDepthStencilDesc(bool flag, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func);
		// ラスタライザ設定
		void SetRasterizerDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill);
		// 描画準備
		void DrawSetting(PSOType type, D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// PSO生成
		void CreatePso(PSOType type, D3D12_ROOT_PARAMETER* rootParameter, UINT rootNum, D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT samplerNum,
			D3D12_CULL_MODE cull, D3D12_FILL_MODE fill,
			D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE  topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		// インプットレイアウトを無くす
		void NoInputLayout() { useInputLayout_ = false; };

	private:
		Command* command_;
		DXGIDevice* DXGIDevice_;
		DXCCompiler* dxcCompiler_;

		// ファイル名
		struct fileName {
			std::wstring filePach;
		};

		// シェーダファイル
		struct ShaderFile
		{
			fileName pixel;
			fileName vertex;
			fileName domain;
			fileName hull;
			fileName geometry;
			fileName amplification;
			fileName mesh;
		};

		// シェイプバルブ
		struct ShaderBlob {
			Microsoft::WRL::ComPtr < IDxcBlob> PS;
			Microsoft::WRL::ComPtr < IDxcBlob> VS;
			Microsoft::WRL::ComPtr < IDxcBlob> DS;
			Microsoft::WRL::ComPtr < IDxcBlob> HS;
			Microsoft::WRL::ComPtr < IDxcBlob> GS;
			Microsoft::WRL::ComPtr < IDxcBlob> AS;
			Microsoft::WRL::ComPtr < IDxcBlob> MS;
		};


		//
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_{};
		std::vector<std::string> semanticNames_; // SemanticName を保持するための vector
		std::vector<UINT> semanticIndex_;
		std::vector<DXGI_FORMAT> semanticformat_;
		std::vector<UINT> semanticSlot_;

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
		D3D12_RASTERIZER_DESC rasterizerDesc_{};


		ShaderFile shderFile_;


		bool useInputLayout_ = true;

	private:
		std::map<PSOType, PSRS> psoRoots_;
	private:
		// グラフィックパイプライン設定
		void SetShederGraphics(D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipeline);
	};
}
