#include "PostEffect.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/PSO/PSOManager.h"

#pragma region Data

void PostEffectData::Initialize(DirectXCommon* dxCommon, PostEffectType type)
{
	type_ = type;
	dxCommon_ = dxCommon;

	vertexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(ScreenVertexData) * 4);
	//リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つの分のサイズ
	vertexBufferView.SizeInBytes = sizeof(ScreenVertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(ScreenVertexData);

	switch (type_)
	{
	case PostEffectType::kCopy:
		break;
	case PostEffectType::kGrayScale:
		break;
	case PostEffectType::kSepia:
		break;
	case PostEffectType::kVignette:
		cbVignette_ = std::make_unique<ConstantBuffer<VignetteGPU>>();
		cbVignette_->CreateBuffer(dxCommon);
		cbVignette_->Data()->scale = 16.0f;
		cbVignette_->Data()->squared = 0.8f;
		break;
	case PostEffectType::kSmoothing:
		cbSmoothig_ = std::make_unique<ConstantBuffer<SmoothigGPU>>();
		cbSmoothig_->CreateBuffer(dxCommon);
		cbSmoothig_->Data()->num = 3;
		break;
	case PostEffectType::kGaussian:
		cbGaussian_ = std::make_unique<ConstantBuffer<GaussianGPU>>();
		cbGaussian_->CreateBuffer(dxCommon);
		cbGaussian_->Data()->num = 3;
		cbGaussian_->Data()->sigma = 2.0f;
		break;
	case PostEffectType::kOitline:
		cbOutline_ = std::make_unique<ConstantBuffer<OutlineGPU>>();
		cbOutline_->CreateBuffer(dxCommon);
		cbOutline_->Data()->num = 3;
		cbOutline_->Data()->weightSquared = 0.002f;
		cbOutline_->Data()->projectionInverse = Identity();
		break;
	case PostEffectType::kRadialBlur:
		cbRadialBlur_ = std::make_unique<ConstantBuffer<RadialBlurGPU>>();
		cbRadialBlur_->CreateBuffer(dxCommon);
		cbRadialBlur_->Data()->center = Vector2{ 0.5f,0.5f };
		cbRadialBlur_->Data()->numSamples = 10;
		cbRadialBlur_->Data()->blurWidth = 0.01f;
		break;
	case PostEffectType::kDissovle:
		cbDissovle_ = std::make_unique<ConstantBuffer<DissovleGPU>>();
		cbDissovle_->CreateBuffer(dxCommon);
		cbDissovle_->Data()->threshold = 0.5f;
		cbDissovle_->Data()->edge = 0.03f;
		cbDissovle_->Data()->color.x = 1.0f;
		cbDissovle_->Data()->color.y = 0.4f;
		cbDissovle_->Data()->color.z = 0.3f;
		break;
	case PostEffectType::kRandom:
		cbRandom_ = std::make_unique<ConstantBuffer<RandomGPU>>();
		cbRandom_->CreateBuffer(dxCommon);
		cbRandom_->Data()->time = 0.0f;
		break;
	case PostEffectType::kBloom:
		cbBloom_ = std::make_unique<ConstantBuffer<BloomGPU>>();
		cbBloom_->CreateBuffer(dxCommon);
		cbBloom_->Data()->threshold = 0.9f;
		cbBloom_->Data()->intensity = 1.0f;
		break;
	case PostEffectType::kBloomCombin:
		break;
	default:
		break;
	}

}

void PostEffectData::DrawRender()
{
	switch (type_
)
	{
	case PostEffectType::kCopy:
		break;
	case PostEffectType::kGrayScale:
		break;
	case PostEffectType::kSepia:
		break;
	case PostEffectType::kVignette:
		cbVignette_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kSmoothing:
		cbSmoothig_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kGaussian:
		cbGaussian_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kOitline:
		if (camera_) {
			cbOutline_->Data()->projectionInverse = Inverse(camera_->GetProjectionMatrix());
			cbOutline_->Data()->nearZ = camera_->GetNearZ();
			cbOutline_->Data()->farZ = camera_->GetFarZ();
		}
		cbOutline_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kRadialBlur:
		cbRadialBlur_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kDissovle:
		cbDissovle_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kRandom:
		cbRandom_->Data()->time += 0.01f;
		cbRandom_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kBloom:
		cbBloom_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kBloomCombin:
		break;
	default:
		break;
	}

	DrawColl();
}

void PostEffectData::UpdateImgui()
{

	switch (type_)
	{
	case PostEffectType::kCopy:
		break;
	case PostEffectType::kGrayScale:
		break;
	case PostEffectType::kSepia:
		break;
	case PostEffectType::kVignette:
		ImGui::DragFloat("scale", &cbVignette_->Data()->scale, 0.01f);
		ImGui::DragFloat("squared", &cbVignette_->Data()->squared, 0.01f);
		break;
	case PostEffectType::kSmoothing:
		if (ImGui::Button("Filter3x3")) {
			cbSmoothig_->Data()->num = 3;
		}
		if (ImGui::Button("Filter5x5")) {
			cbSmoothig_->Data()->num = 5;
		}
		break;
	case PostEffectType::kGaussian:
		if (ImGui::Button("Filter3x3")) {
			cbGaussian_->Data()->num = 3;
		}
		if (ImGui::Button("Filter5x5")) {
			cbGaussian_->Data()->num = 5;
		}
		if (ImGui::Button("Filter7x7")) {
			cbGaussian_->Data()->num = 7;
		}
		if (ImGui::Button("Filter9x9")) {
			cbGaussian_->Data()->num = 9;
		}
		ImGui::DragFloat("sigma", &cbGaussian_->Data()->sigma);
		break;
	case PostEffectType::kOitline:
		if (ImGui::Button("0")) {
			cbOutline_->Data()->num = 0;
		}
		if (ImGui::Button("1")) {
			cbOutline_->Data()->num = 1;
		}
		if (ImGui::Button("2")) {
			cbOutline_->Data()->num = 2;
		}
		if (ImGui::Button("3")) {
			cbOutline_->Data()->num = 3;
		}
		ImGui::DragFloat("squared", &cbOutline_->Data()->weightSquared, 0.1f);
		break;
	case PostEffectType::kRadialBlur:
		ImGui::DragFloat2("scale", &cbRadialBlur_->Data()->center.x, 0.01f);
		ImGui::DragFloat("blurWidth", &cbRadialBlur_->Data()->blurWidth, 0.01f);
		ImGui::SliderInt("numSamples", &cbRadialBlur_->Data()->numSamples, 1, 20);
		break;
	case PostEffectType::kDissovle:
		ImGui::DragFloat("threshold", &cbDissovle_->Data()->threshold, 0.01f);
		ImGui::DragFloat("edge", &cbDissovle_->Data()->edge, 0.001f);
		ImGui::ColorEdit3("color", &cbDissovle_->Data()->color.x);
		break;
	case PostEffectType::kRandom:
		break;
	case PostEffectType::kBloom:
		ImGui::SliderFloat("threshold", &cbBloom_->Data()->threshold, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &cbBloom_->Data()->intensity, 0.01f);
		break;
	case PostEffectType::kBloomCombin:
		break;
	default:
		break;
	}

}


void PostEffectData::DrawColl()
{
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

#pragma endregion

#pragma region Base

void IPostEffect::Initialize(DirectXCommon* dxCommon, std::string psName) {
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());
	// ルートシグネチャ生成
	CreateRootSignature();
	// パイプライン生成
	CreateCommonPipeline(psName);
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

#pragma endregion

#pragma region Copy

void PostEffectCopy::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

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

#pragma endregion

#pragma region GrayScale

void PostEffectGrayScale::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

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

#pragma endregion

#pragma region Sepia

void PostEffectSepia::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

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

#pragma endregion

#pragma region Vignette

void PostEffectVignette::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void PostEffectVignette::CreateRootSignature() {
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

#pragma endregion

#pragma region Smoothing

void PostEffectSmoothing::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void PostEffectSmoothing::CreateRootSignature() {
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

#pragma endregion

#pragma region Gaussian

void PostEffectGaussian::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void PostEffectGaussian::CreateRootSignature() {
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

#pragma endregion

#pragma region Outline

void PostEffectOutline::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, dxCommon_->GetDepthStencil()->GetDepthSrvIndex());
}

void PostEffectOutline::CreateRootSignature() {
	// アウトライン	
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutline[2] = {};
	PSOFanction::SetDescriptorRenge(descriptorRangeOutline[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ
	PSOFanction::SetDescriptorRenge(descriptorRangeOutline[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Depth用

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersOutline[2] = {};
	PSOFanction::SetSampler(staticSamplersOutline[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);
	PSOFanction::SetSampler(staticSamplersOutline[1], 1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER outlineRootParameters[3] = {};

	PSOFanction::SetRootParameter(outlineRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(outlineRootParameters[1], descriptorRangeOutline[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(outlineRootParameters[2], descriptorRangeOutline[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, outlineRootParameters, _countof(outlineRootParameters), staticSamplersOutline, _countof(staticSamplersOutline));

}

#pragma endregion

#pragma region RadialBlur

void PostEffectRadialBlur::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void PostEffectRadialBlur::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	PSOFanction::SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, TextureAddressMode::kCLAMP);

	D3D12_ROOT_PARAMETER RootParameters[2] = {};
	//ラジアルブラー (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(RootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(RootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, RootParameters, _countof(RootParameters), staticSamplersBlur, _countof(staticSamplersBlur));
}

#pragma endregion

#pragma region Dissovle

void PostEffectDissovle::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, dissovleIndex);
}

void PostEffectDissovle::CreateRootSignature() {
	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);


	// ディゾルブ
	D3D12_DESCRIPTOR_RANGE descriptorRangeDissovle[2] = {};
	PSOFanction::SetDescriptorRenge(descriptorRangeDissovle[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ
	PSOFanction::SetDescriptorRenge(descriptorRangeDissovle[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Depth用

	D3D12_ROOT_PARAMETER dissovleRootParameters[3] = {};
	//　ディゾルブ(b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(dissovleRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(dissovleRootParameters[1], descriptorRangeDissovle[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(dissovleRootParameters[2], descriptorRangeDissovle[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, dissovleRootParameters, _countof(dissovleRootParameters), staticSamplers, _countof(staticSamplers));

	// ノイズテクスチャ
	dxCommon_->GetTextureManager()->LoadTexture("resources/Texture/noise.jpg");
	dissovleIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath("resources/Texture/noise.jpg");
}

#pragma endregion

#pragma region Random

void PostEffectRandom::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void PostEffectRandom::CreateRootSignature() {
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



	// ランダム(b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region Bloom

void PostEffectBloom::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void PostEffectBloom::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	PSOFanction::SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, TextureAddressMode::kCLAMP);

	D3D12_ROOT_PARAMETER RootParameters[2] = {};
	// ブルーム (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(RootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(RootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, RootParameters, _countof(RootParameters), staticSamplersBlur, _countof(staticSamplersBlur));
}

#pragma endregion

#pragma region Combin

void PostEffectCombin::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, indexB);
}

void PostEffectCombin::CreateRootSignature() {
	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	PSOFanction::SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, TextureAddressMode::kCLAMP);


	D3D12_DESCRIPTOR_RANGE descriptorRangeCombin[2] = {};
	PSOFanction::SetDescriptorRenge(descriptorRangeCombin[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	PSOFanction::SetDescriptorRenge(descriptorRangeCombin[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	D3D12_ROOT_PARAMETER RootParametersCombin[3] = {};
	// ブルーム (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(RootParametersCombin[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(RootParametersCombin[1], descriptorRangeCombin[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(RootParametersCombin[2], descriptorRangeCombin[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, RootParametersCombin, _countof(RootParametersCombin), staticSamplersBlur, _countof(staticSamplersBlur));
}

#pragma endregion












