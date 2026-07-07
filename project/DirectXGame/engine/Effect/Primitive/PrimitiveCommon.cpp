#include "PrimitiveCommon.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::PrimitiveCommon::Initialize(DirectXCommon* dxcommon)
{
	this->dxCommon = dxcommon;	// DX共通クラス

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());

	// パイプライン生成
	CreateGraphicsPipeline();
}

void Engine::PrimitiveCommon::DrawCommonSetting(PsoType type)
{
	// タイプに合わせてパイプライン選択
	switch (type)
	{
	case PrimitiveCommon::PsoType::kDefault:
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(default_.rootSignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(default_.pipelineState.Get()); //PSOを設定
		break;
	case PrimitiveCommon::PsoType::kRingClamp:
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(defaultRing_.rootSignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(defaultRing_.pipelineState.Get()); //PSOを設定
		break;
	case PrimitiveCommon::PsoType::kNoCull:
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(noCull_.rootSignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(noCull_.pipelineState.Get()); //PSOを設定
		break;
	case PrimitiveCommon::PsoType::kNoCullRingClamp:
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(noCullRing_.rootSignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(noCullRing_.pipelineState.Get()); //PSOを設定
		break;
	case PrimitiveCommon::PsoType::kNoCullWireFrame:
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(noCullWireFrame_.rootSignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(noCullWireFrame_.pipelineState.Get()); //PSOを設定
		break;
	default:
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(default_.rootSignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(default_.pipelineState.Get()); //PSOを設定
		break;
	}

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::PrimitiveCommon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);



	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	// マテリアル
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);

	// トランスフォーム
	PSOFunction::SetRootParameter(rootParameters[1], 1, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);

	// テクスチャ用
	PSOFunction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);
	psoManager_->SetRootSignature(default_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
	psoManager_->SetRootSignature(noCull_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
	psoManager_->SetRootSignature(noCullWireFrame_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));


	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	psoManager_->SetRootSignature(defaultRing_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
	psoManager_->SetRootSignature(noCullRing_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

}

void Engine::PrimitiveCommon::CreateGraphicsPipeline()
{
	CreateRootSignature();





#pragma region BlendState

	// BlendState(ブレンドステート)の設定
	// 標準のアルファブレンド設定を生成する
	D3D12_BLEND_DESC blendDesc = PSOFunction::CreateAlphaBlendDesc();

#pragma endregion //BlendState(ブレンドステート)

	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFunction::CreateDepthStencilDesc();


	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Trail/Trail.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Trail/Trail.PS.hlsl");


	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(default_.rootSignature, default_.pipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(defaultRing_.rootSignature, defaultRing_.pipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(noCull_.rootSignature, noCull_.pipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(noCullRing_.rootSignature, noCullRing_.pipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME);
	psoManager_->GraphicsPipelineState(noCullWireFrame_.rootSignature, noCullWireFrame_.pipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


}
