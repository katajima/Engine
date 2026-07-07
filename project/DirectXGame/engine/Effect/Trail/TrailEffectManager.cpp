#include "TrailEffectManager.h"

#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::TrailEffectCommon::Initialize(DirectXCommon* dxcommon)
{
	this->dxCommon = dxcommon;		// DX共通クラス

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());
	
	// パイプライン生成
	CreateGraphicsPipeline();
}

void Engine::TrailEffectCommon::DrawCommonSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::TrailEffectCommon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0],0,1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);


	
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	
	// マテリアル
	PSOFunction::SetRootParameter(rootParameters[0],0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	
	// トランスフォーム
	PSOFunction::SetRootParameter(rootParameters[1],1, D3D12_SHADER_VISIBILITY_VERTEX,D3D12_ROOT_PARAMETER_TYPE_CBV);
	
	// テクスチャ用
	PSOFunction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0],0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

	
}

void Engine::TrailEffectCommon::CreateGraphicsPipeline()
{
	CreateRootSignature();




#pragma region BlendState

	// BlendState(ブレンドステート)の設定
	// 標準のアルファブレンド設定を生成する
	D3D12_BLEND_DESC blendDesc = PSOFunction::CreateAlphaBlendDesc();

#pragma endregion //BlendState(ブレンドステート)

	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFunction::CreateDepthStencilDesc();
	// 半透明の帯同士が深度を書いて欠けないよう、テストだけ行って書き込みはしない。
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;


	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Trail/Trail.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Trail/Trail.PS.hlsl");

	
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

}
