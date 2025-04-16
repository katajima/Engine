#include "RenderingCommon.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void RenderingCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());
	CreateGraphicsPipeline();

	vertexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(ScreenVertexData) * 4);

	
	//リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つの分のサイズ
	vertexBufferView.SizeInBytes = sizeof(ScreenVertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(ScreenVertexData);

	
}

void RenderingCommon::DrawCommonSetting(int index)
{
	OutputDebugStringA("DrawInstanced() called\n");

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); //PSOを設定

	// 状態遷移が正常かを確認
	OutputDebugStringA(">>> Drawing with RenderTexture in RENDER_TARGET state\n");


	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);


	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定

	
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void RenderingCommon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));


}

void RenderingCommon::CreateGraphicsPipeline()
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

	// InputLayout(インプットレイアウト)
	// VectorShaderへ渡す頂点データがどのようなものかを指定するオブジェクト

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);


	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Offscreen/Fullscreen.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/CopyImage.PS.hlsl";

	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;


	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState, blendDesc, depthStencilDesc);

}
