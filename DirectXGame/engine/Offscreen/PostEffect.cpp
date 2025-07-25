#include "PostEffect.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Camera/Camera.h"


#pragma region Base

void IPostEffect::Initialize(DirectXCommon* dxCommon, std::string psName) {
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());


	// リソース生成
	CreateBuffer();

	// ルートシグネチャ生成
	CreateRootSignature();

	// パイプライン生成
	CreateCommonPipeline(psName);
	
	vertexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(ScreenVertexData) * 4);
	//リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つの分のサイズ
	vertexBufferView.SizeInBytes = sizeof(ScreenVertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(ScreenVertexData);
};

// 共通パイプライン生成
void IPostEffect::CreateCommonPipeline(std::string psName)
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

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;


	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Offscreen/Fullscreen.VS.hlsl");

	std::string name = "resources/shaders/Offscreen/" + psName + ".PS.hlsl";

	psoManager_->SetShaderFileName(ShaderFileName::PS, StringUtility::ConvertString(name));
	psoManager_->GraphicsPipelineState(posteffect_.rootSignature, posteffect_.graphicsPipelineState, blendDesc, depthStencilDesc);

}

// 描画設定
void IPostEffect::DrawSetting()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(posteffect_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(posteffect_.graphicsPipelineState.Get()); //PSOを設定

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void IPostEffect::DrawColl()
{
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

#pragma endregion

#pragma region Copy

void PostEffectCopy::DrawRender(int index, int indexB)
{
	DrawSetting();

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void PostEffectCopy::UpdateImgui() {}

void PostEffectCopy::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

void PostEffectCopy::CreateBuffer(){}

#pragma endregion

#pragma region GrayScale

void PostEffectGrayScale::DrawRender(int index, int indexB)
{
	DrawSetting();

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void PostEffectGrayScale::UpdateImgui() {}

void PostEffectGrayScale::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

void PostEffectGrayScale::CreateBuffer(){}

#pragma endregion

#pragma region Sepia

void PostEffectSepia::DrawRender(int index, int indexB)
{
	DrawSetting();

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void PostEffectSepia::UpdateImgui() {}

void PostEffectSepia::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

void PostEffectSepia::CreateBuffer() {}

#pragma endregion









