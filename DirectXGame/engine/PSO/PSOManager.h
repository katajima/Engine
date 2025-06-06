#pragma once

#include "PSOFanction.h"


#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"


class Command;
class DXGIDevice;
class DXCCompiler;




class PSOManager
{
public:
	// パイプラインステート＋ルートシグネチャ
	struct PSRS {
		////ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		//// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;
	};




	// 初期化
	void Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler);


	void SetRootSignature(
		Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
		D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters,
		D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT numSamplers);


	void GraphicsPipelineState(Microsoft::WRL::ComPtr < ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr < ID3D12PipelineState>& graphicsPipelineState
		, D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE  topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


	void AddInputElementDesc(const std::string& semanticName, UINT semanticIndex, DXGI_FORMAT format,UINT slot = 0);



	void SetDepthStencilDesc(bool flag, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func);

	void SetRasterizerDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill);

	void DrawSetting(D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

private:
	Command* command_;
	DXGIDevice* DXGIDevice_;
	DXCCompiler* dxcCompiler_;

	struct fileName {
		std::wstring filePach;
	};


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
	

public:
	ShaderFile shderFile_;
private:
	PSRS psoRoot_;


private:
	
	void SetShederGraphics(D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipeline);

};

