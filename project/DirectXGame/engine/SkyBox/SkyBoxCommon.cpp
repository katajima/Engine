#include "SkyBoxCommon.h"

#include "DirectXGame/engine/Transform/Transformation/Transformation.h"
#include "DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::SkyBoxCommon::Initialize(DirectXCommon* dxCommon)
{
	this->dxCommon = dxCommon;

	// PSOマネジャー
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());

	// グラフィックパイプライン生成
	CreateGraphicsPipeline();

}



void Engine::SkyBoxCommon::DrawCommonSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::SkyBoxCommon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0],0,1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0],0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);

	// トランスフォームデータ (b0) を頂点シェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1],0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);

	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	PSOFunction::SetSampler(staticSamplers[0],0 ,D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	
	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature,rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
	

}

void Engine::SkyBoxCommon::CreateGraphicsPipeline()
{
	CreateRootSignature();

#pragma region BlendState

	// BlendState(ブレンドステート)の設定
	// 標準のアルファブレンド設定を生成する
	D3D12_BLEND_DESC blendDesc = PSOFunction::CreateAlphaBlendDesc();

#pragma endregion //BlendState(ブレンドステート)

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK,D3D12_FILL_MODE_SOLID);

	
	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/SkyBox/SkyBox.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/SkyBox/SkyBox.PS.hlsl");


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFunction::CreateDepthStencilDesc();
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;



	psoManager_->GraphicsPipelineState(rootSignature,graphicsPipelineState,blendDesc, depthStencilDesc);
	
}
