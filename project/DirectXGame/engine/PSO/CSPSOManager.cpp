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

void CSPSOManager::SetRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters)
{
	//Roosignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	descriptionSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionSignature.pParameters = rootParameter;
	descriptionSignature.NumParameters = numRootParameters;

	PSOFanction::Blob(DXGIDevice_,descriptionSignature, computePSRS_.rootSignature);
}

void CSPSOManager::ComputePipelineState()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};


	SetShederCompute(computePipelineStateDesc);

	computePipelineStateDesc.pRootSignature = computePSRS_.rootSignature.Get();// RootSignature


	
	hr_ = DXGIDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&computePSRS_.computePipelineState));

}

void CSPSOManager::PreComputePSRS()
{
	command_->GetList()->SetComputeRootSignature(computePSRS_.rootSignature.Get());
	command_->GetList()->SetPipelineState(computePSRS_.computePipelineState.Get());
}

void CSPSOManager::SetShaderFileName(std::wstring filename)
{
	shderFile_.commpute.filePach = filename;
}

void CSPSOManager::SetShederCompute(D3D12_COMPUTE_PIPELINE_STATE_DESC& graphicsPipeline)
{
	if (shderFile_.commpute.filePach != L"") {
		// Shaderをコンパイルする
		CS = dxcCompiler_->CompileShader(shderFile_.commpute.filePach,
			L"cs_6_0");
		assert(CS != nullptr);
		graphicsPipeline.CS = { CS->GetBufferPointer(),
		CS->GetBufferSize() }; // VertexShader
	}
}

