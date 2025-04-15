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

class CSPSOManager
{
public:
	// 初期化
	void Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler);

	void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister, D3D12_SHADER_VISIBILITY shaderType, D3D12_ROOT_PARAMETER_TYPE rootType);
	void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, D3D12_DESCRIPTOR_RANGE& descriptorRange, D3D12_SHADER_VISIBILITY shaderType);

	void SetDescriptorRenge(D3D12_DESCRIPTOR_RANGE& descriptorRange, int ShaderRegister, int numDescriptors, D3D12_DESCRIPTOR_RANGE_TYPE rengeType);

	void SetRootSignature(
		Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
		D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters,
		D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT numSamplers);


	void ComputePipelineState(Microsoft::WRL::ComPtr <ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr < ID3D12PipelineState>& graphicsPipelineState);
private:
	Command* command_;
	DXGIDevice* DXGIDevice_;
	DXCCompiler* dxcCompiler_;


	struct fileName {
		std::wstring filePach;
	};


	struct ShaderFile
	{
		fileName commpute;
	};

	Microsoft::WRL::ComPtr < IDxcBlob> CS;

	HRESULT hr_;
public:
	ShaderFile shderFile_;


public:
	void Blob(D3D12_ROOT_SIGNATURE_DESC descriptionSignature, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature);

	
	void SetShederCompute(D3D12_COMPUTE_PIPELINE_STATE_DESC& graphicsPipeline);
};