#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<cstdint>
#include<wrl.h>
using namespace Microsoft::WRL;
#include<list>
#include<string>
#include<vector>
#include<format>


#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"


class Command;
class DXGIDevice;
class DXCCompiler;

class PSOManager
{
public:
	// 初期化
	void Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler);



	

	void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister, D3D12_SHADER_VISIBILITY shaderType, D3D12_ROOT_PARAMETER_TYPE rootType);
	void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, D3D12_DESCRIPTOR_RANGE& descriptorRange, D3D12_SHADER_VISIBILITY shaderType);


	void SetDescriptorRenge(D3D12_DESCRIPTOR_RANGE& descriptorRange, int ShaderRegister,int numDescriptors, D3D12_DESCRIPTOR_RANGE_TYPE rengeType);


	void SetSampler(D3D12_STATIC_SAMPLER_DESC& staticSamplers, int shaderRegister, D3D12_FILTER filter, D3D12_SHADER_VISIBILITY shaderType);


	void SetRootSignature(
		Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
		D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters,
		D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT numSamplers);




	void GraphicsPipelineState(Microsoft::WRL::ComPtr < ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr < ID3D12PipelineState>& graphicsPipelineState
		, D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE  topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


	void AddInputElementDesc(const std::string& semanticName, UINT semanticIndex, DXGI_FORMAT format,UINT slot = 0);



	void SetDepthStencilDesc(bool flag, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func);

	void SetRasterizerDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill);


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



private:
	void Blob(D3D12_ROOT_SIGNATURE_DESC descriptionSignature, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature);

	void SetShederGraphics(D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipeline);

};

