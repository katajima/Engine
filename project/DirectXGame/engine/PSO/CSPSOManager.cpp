#include "CSPSOManager.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/DXCCompiler/DXCCompiler.h"

void Engine::CSPSOManager::Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler)
{
	this->command = command;
	this->dxgiDevice = DXGIDevice;
	this->dxcCompiler = dxcCompiler;
}

void Engine::CSPSOManager::SetRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters)
{
	//Roosignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	descriptionSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionSignature.pParameters = rootParameter;
	descriptionSignature.NumParameters = numRootParameters;

	PSOFanction::Blob(dxgiDevice,descriptionSignature, computePSRS_.rootSignature);
}

void Engine::CSPSOManager::ComputePipelineState()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};


	SetShederCompute(computePipelineStateDesc);

	computePipelineStateDesc.pRootSignature = computePSRS_.rootSignature.Get();// RootSignature


	
	hr_ = dxgiDevice->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&computePSRS_.computePipelineState));

}

void Engine::CSPSOManager::PreComputePSRS()
{
	command->GetList()->SetComputeRootSignature(computePSRS_.rootSignature.Get());
	command->GetList()->SetPipelineState(computePSRS_.computePipelineState.Get());
}

void Engine::CSPSOManager::SetShaderFileName(std::wstring filename)
{
	shderFile_.commpute.filePach = filename;
}

void Engine::CSPSOManager::SetShederCompute(D3D12_COMPUTE_PIPELINE_STATE_DESC& graphicsPipeline)
{
	if (shderFile_.commpute.filePach != L"") {
		// Shaderをコンパイルする
		CS = dxcCompiler->CompileShader(shderFile_.commpute.filePach,
			L"cs_6_0");
		assert(CS != nullptr);
		graphicsPipeline.CS = { CS->GetBufferPointer(),
		CS->GetBufferSize() }; // VertexShader
	}
}

