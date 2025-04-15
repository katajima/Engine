#include "CSPSOManager.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/DXCCompiler/DXCCompiler.h"

void CSPSOManager::Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler)
{
	command_ = command;
	DXGIDevice_ = DXGIDevice;
	dxcCompiler_ = dxcCompiler;
}

void CSPSOManager::SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister, D3D12_SHADER_VISIBILITY shaderType, D3D12_ROOT_PARAMETER_TYPE rootType)
{
	// どのシェーダを使うのか
	parameter.ShaderVisibility = shaderType;

	// どのパラメータタイプを使うのか
	parameter.ParameterType = rootType;

	// レジスタ番号
	parameter.Descriptor.ShaderRegister = ShaderRegister;    // レジスタ番号0とバインド　　// b0の0と一致する。もしb11と紐づけたいなら11となる
}

void CSPSOManager::SetRootParameter(D3D12_ROOT_PARAMETER& parameter, D3D12_DESCRIPTOR_RANGE& descriptorRange, D3D12_SHADER_VISIBILITY shaderType)
{
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う           
	parameter.ShaderVisibility = shaderType;
	parameter.DescriptorTable.pDescriptorRanges = &descriptorRange; // Tableの中身の配列を指定
	parameter.DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数 
}

void CSPSOManager::SetDescriptorRenge(D3D12_DESCRIPTOR_RANGE& descriptorRange, int ShaderRegister, int numDescriptors, D3D12_DESCRIPTOR_RANGE_TYPE rengeType)
{
	descriptorRange.BaseShaderRegister = ShaderRegister; //
	descriptorRange.RegisterSpace = 0; // レジスタスペース (通常は0) 
	descriptorRange.NumDescriptors = numDescriptors; // 数は1つ
	descriptorRange.RangeType = rengeType;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
}

void CSPSOManager::SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature, D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters, D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT numSamplers)
{
	//Roosignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	descriptionSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionSignature.pParameters = rootParameter;
	descriptionSignature.NumParameters = numRootParameters;
	descriptionSignature.pStaticSamplers = samplerDesc;
	descriptionSignature.NumStaticSamplers = numSamplers;

	Blob(descriptionSignature, rootSignature);
}

void CSPSOManager::Blob(D3D12_ROOT_SIGNATURE_DESC descriptionSignature, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature)
{
	HRESULT hr;
	// descriptionSignature が正しく設定されているか確認
	if (descriptionSignature.pParameters == nullptr || descriptionSignature.NumParameters == 0)
	{
		Logger::Log("descriptionSignature is not properly set.");
		assert(false);
		return;
	}
	//シリアライズにしてバイナリする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));

		assert(false);
	}
	//バイナリを元に生成
	hr = DXGIDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}

void CSPSOManager::SetShederGraphics(D3D12_COMPUTE_PIPELINE_STATE_DESC& graphicsPipeline)
{

	if (shderFile_.commpute.filePach != L"") {
		// Shaderをコンパイルする
		CS = dxcCompiler_->CompileShader(shderFile_.commpute.filePach,
			L"vs_6_0");
		assert(CS != nullptr);
		graphicsPipeline.CS = { CS->GetBufferPointer(),
		CS->GetBufferSize() }; // VertexShader
	}
}

