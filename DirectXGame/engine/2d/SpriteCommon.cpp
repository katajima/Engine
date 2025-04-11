#include"SpriteCommon.h"

SpriteCommon* SpriteCommon::instance = nullptr;

SpriteCommon* SpriteCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new SpriteCommon;
	}
	return instance;
}

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	psoManager_ = dxCommon_->GetPSOManager();
	
	CreateGraphicsPipeline();
}

void SpriteCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void SpriteCommon::DrawCommonSetting(PSOType type)
{
	switch (type)
	{
	case SpriteCommon::PSOType::UvInterpolation_MODE_SOLID:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get()); //PSOを設定
		break;
	case SpriteCommon::PSOType::NoUvInterpolation_MODE_SOLID:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get()); //PSOを設定
		break;
	case SpriteCommon::PSOType::UvInterpolation_MODE_WIREFRAME:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get()); //PSOを設定
		break;
	case SpriteCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get()); //PSOを設定
		break;
	default:
		break;
	}

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}



void SpriteCommon::CreateRootSignature()
{


	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);



	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	// マテリアル
	psoManager_->SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);

	// トランスフォーム
	psoManager_->SetRootParameter(rootParameters[1], 0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);

	// テクスチャ用
	psoManager_->SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	
	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature[0], rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));


	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature[1], rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

}


void SpriteCommon::CreateGraphicsPipeline()
{
	CreateRootSignature();

#pragma region BlendState


	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

#pragma endregion //BlendState(ブレンドステート)


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Object2D/Object2D.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Object2D/Object2D.PS.hlsl";


	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[0], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[1], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[2], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[3], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}

