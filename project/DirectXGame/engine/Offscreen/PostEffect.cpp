#include "PostEffect.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/PSO/PSOManager.h"

#pragma region Data

void Engine::PostEffectData::Initialize(DirectXCommon* dxCommon, PostEffectType type)
{
	type_ = type;
	this->dxCommon = dxCommon;

	vertexResource = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(ScreenVertexData) * 4);
	// LiveObject表示でポストエフェクトの頂点バッファを判別できるよう名前を付ける。
	vertexResource->SetName(L"PostEffect Screen Vertex");
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
		cbVignette_ = std::make_unique<Engine::ConstantBuffer<VignetteGPU>>();
		cbVignette_->CreateBuffer(dxCommon);
		cbVignette_->SetResourceName(L"PostEffect Vignette CB");
		cbVignette_->Data()->scale = 16.0f;
		cbVignette_->Data()->squared = 0.8f;
		cbVignette_->Data()->color = { 1,1,1,1 };
		break;
	case PostEffectType::kSmoothing:
		cbSmoothig_ = std::make_unique<Engine::ConstantBuffer<SmoothigGPU>>();
		cbSmoothig_->CreateBuffer(dxCommon);
		cbSmoothig_->SetResourceName(L"PostEffect Smoothing CB");
		cbSmoothig_->Data()->num = 3;
		break;
	case PostEffectType::kGaussian:
		cbGaussian_ = std::make_unique<Engine::ConstantBuffer<GaussianGPU>>();
		cbGaussian_->CreateBuffer(dxCommon);
		cbGaussian_->SetResourceName(L"PostEffect Gaussian CB");
		cbGaussian_->Data()->num = 3;
		cbGaussian_->Data()->sigma = 2.0f;
		break;
	case PostEffectType::kOitline:
		cbOutline_ = std::make_unique<Engine::ConstantBuffer<OutlineGPU>>();
		cbOutline_->CreateBuffer(dxCommon);
		cbOutline_->SetResourceName(L"PostEffect Outline CB");
		cbOutline_->Data()->num = 3;
		cbOutline_->Data()->weightSquared = 0.002f;
		cbOutline_->Data()->projectionInverse = Identity();
		break;
	case PostEffectType::kRadialBlur:
		cbRadialBlur_ = std::make_unique<Engine::ConstantBuffer<RadialBlurGPU>>();
		cbRadialBlur_->CreateBuffer(dxCommon);
		cbRadialBlur_->SetResourceName(L"PostEffect RadialBlur CB");
		cbRadialBlur_->Data()->center = Vector2{ 0.5f,0.5f };
		cbRadialBlur_->Data()->numSamples = 10;
		cbRadialBlur_->Data()->blurWidth = 0.01f;
		break;
	case PostEffectType::kDissolve:
		cbDissolve_ = std::make_unique<Engine::ConstantBuffer<DissolveGPU>>();
		cbDissolve_->CreateBuffer(dxCommon);
		cbDissolve_->SetResourceName(L"PostEffect Dissolve CB");
		cbDissolve_->Data()->threshold = 0.5f;
		cbDissolve_->Data()->edge = 0.03f;
		cbDissolve_->Data()->color.x = 1.0f;
		cbDissolve_->Data()->color.y = 0.4f;
		cbDissolve_->Data()->color.z = 0.3f;
		break;
	case PostEffectType::kRandom:
		cbRandom_ = std::make_unique<Engine::ConstantBuffer<RandomGPU>>();
		cbRandom_->CreateBuffer(dxCommon);
		cbRandom_->SetResourceName(L"PostEffect Random CB");
		cbRandom_->Data()->time = 0.0f;
		break;
	case PostEffectType::kBloom:
		cbBloom_ = std::make_unique<Engine::ConstantBuffer<BloomGPU>>();
		cbBloom_->CreateBuffer(dxCommon);
		cbBloom_->SetResourceName(L"PostEffect Bloom CB");
		cbBloom_->Data()->threshold = 0.9f;
		cbBloom_->Data()->intensity = 1.0f;
		break;
	case PostEffectType::kBloomCombine:
		break;
	default:
		break;
	}

}

void Engine::PostEffectData::DrawRender()
{
	// 各タイプに応じてGPUに送るデータ設定
	switch (type_)
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
		if (camera) {
			cbOutline_->Data()->projectionInverse = Inverse(camera->GetProjectionMatrix());
			cbOutline_->Data()->nearZ = camera->GetNearZ();
			cbOutline_->Data()->farZ = camera->GetFarZ();
		}
		cbOutline_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kRadialBlur:
		cbRadialBlur_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kDissolve:
		cbDissolve_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kRandom:
		cbRandom_->Data()->time += 0.01f;
		cbRandom_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kBloom:
		cbBloom_->SetGraphicsRootConstantBufferView(0);
		break;
	case PostEffectType::kBloomCombine:
		break;
	default:
		break;
	}

	DrawColl();
}

void Engine::PostEffectData::UpdateImgui()
{
	// 各タイプに応じてImGui更新
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
		ImGui::ColorEdit3("color", &cbVignette_->Data()->color.x);
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
	case PostEffectType::kDissolve:
		ImGui::DragFloat("threshold", &cbDissolve_->Data()->threshold, 0.01f);
		ImGui::DragFloat("edge", &cbDissolve_->Data()->edge, 0.001f);
		ImGui::ColorEdit3("color", &cbDissolve_->Data()->color.x);
		break;
	case PostEffectType::kRandom:
		break;
	case PostEffectType::kBloom:
		ImGui::SliderFloat("threshold", &cbBloom_->Data()->threshold, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &cbBloom_->Data()->intensity, 0.01f);
		break;
	case PostEffectType::kBloomCombine:
		break;
	default:
		break;
	}

}


void Engine::PostEffectData::DrawColl()
{
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
	dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

#pragma endregion

#pragma region Base

void Engine::IPostEffect::Initialize(DirectXCommon* dxCommon, std::string psName) {
	dxCommon_ = dxCommon;	// DX共通クラス
	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());
	// ルートシグネチャ生成
	CreateRootSignature();
	// パイプライン生成
	CreateCommonPipeline(psName);
};

// 共通パイプライン生成
void Engine::IPostEffect::CreateCommonPipeline(std::string psName)
{
	CreateRootSignature();

#pragma region BlendState
	// 標準のアルファブレンド設定を生成する
	D3D12_BLEND_DESC blendDesc = PSOFunction::CreateAlphaBlendDesc();
#pragma endregion //BlendState(ブレンドステート)

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFunction::CreateDepthStencilDesc();
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;


	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Offscreen/Fullscreen.VS.hlsl");

	std::string name = "resources/shaders/Offscreen/" + psName + ".PS.hlsl";

	psoManager_->SetShaderFileName(ShaderFileName::PS, StringUtility::ConvertString(name));
	psoManager_->GraphicsPipelineState(posteffect_.rootSignature, posteffect_.pipelineState, blendDesc, depthStencilDesc);

}

// 描画設定
void Engine::IPostEffect::DrawSetting()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(posteffect_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(posteffect_.pipelineState.Get()); //PSOを設定

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#pragma endregion

#pragma region Copy

void Engine::PostEffectCopy::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectCopy::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region GrayScale

void Engine::PostEffectGrayScale::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectGrayScale::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region Sepia

void Engine::PostEffectSepia::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectSepia::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region Vignette

void Engine::PostEffectVignette::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectVignette::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region Smoothing

void Engine::PostEffectSmoothing::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectSmoothing::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region Gaussian

void Engine::PostEffectGaussian::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectGaussian::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region Outline

void Engine::PostEffectOutline::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, dxCommon_->GetDepthStencil()->GetDepthSrvIndex());
}

void Engine::PostEffectOutline::CreateRootSignature() {
	// アウトライン	
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutline[2] = {};
	PSOFunction::SetDescriptorRange(descriptorRangeOutline[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ
	PSOFunction::SetDescriptorRange(descriptorRangeOutline[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Depth用

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersOutline[2] = {};
	PSOFunction::SetSampler(staticSamplersOutline[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);
	PSOFunction::SetSampler(staticSamplersOutline[1], 1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER outlineRootParameters[3] = {};

	PSOFunction::SetRootParameter(outlineRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(outlineRootParameters[1], descriptorRangeOutline[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(outlineRootParameters[2], descriptorRangeOutline[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, outlineRootParameters, _countof(outlineRootParameters), staticSamplersOutline, _countof(staticSamplersOutline));

}

#pragma endregion

#pragma region RadialBlur

void Engine::PostEffectRadialBlur::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectRadialBlur::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	PSOFunction::SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, TextureAddressMode::kCLAMP);

	D3D12_ROOT_PARAMETER RootParameters[2] = {};
	//ラジアルブラー (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(RootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(RootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, RootParameters, _countof(RootParameters), staticSamplersBlur, _countof(staticSamplersBlur));
}

#pragma endregion

#pragma region Dissolve

void Engine::PostEffectDissolve::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, dissolveIndex);
}

void Engine::PostEffectDissolve::CreateRootSignature() {
	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);


	// ディゾルブ
	D3D12_DESCRIPTOR_RANGE descriptorRangeDissolve[2] = {};
	PSOFunction::SetDescriptorRange(descriptorRangeDissolve[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ
	PSOFunction::SetDescriptorRange(descriptorRangeDissolve[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Depth用

	D3D12_ROOT_PARAMETER dissolveRootParameters[3] = {};
	//　ディゾルブ(b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(dissolveRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(dissolveRootParameters[1], descriptorRangeDissolve[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(dissolveRootParameters[2], descriptorRangeDissolve[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, dissolveRootParameters, _countof(dissolveRootParameters), staticSamplers, _countof(staticSamplers));

	// ノイズテクスチャ
	dxCommon_->GetTextureManager()->LoadTexture("resources/Texture/noise.dds");
	dissolveIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath("resources/Texture/noise.dds");
}

#pragma endregion

#pragma region Random

void Engine::PostEffectRandom::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectRandom::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFunction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);



	// ランダム(b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

#pragma endregion

#pragma region Bloom

void Engine::PostEffectBloom::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
}

void Engine::PostEffectBloom::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFunction::SetDescriptorRange(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	PSOFunction::SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, TextureAddressMode::kCLAMP);

	D3D12_ROOT_PARAMETER RootParameters[2] = {};
	// ブルーム (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(RootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(RootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, RootParameters, _countof(RootParameters), staticSamplersBlur, _countof(staticSamplersBlur));
}

#pragma endregion

#pragma region Combine

void Engine::PostEffectCombine::DrawRender(int index, int indexB)
{
	DrawSetting();
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);
	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, indexB);
}

void Engine::PostEffectCombine::CreateRootSignature() {
	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	PSOFunction::SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, TextureAddressMode::kCLAMP);


	D3D12_DESCRIPTOR_RANGE descriptorRangeCombine[2] = {};
	PSOFunction::SetDescriptorRange(descriptorRangeCombine[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	PSOFunction::SetDescriptorRange(descriptorRangeCombine[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	D3D12_ROOT_PARAMETER RootParametersCombine[3] = {};
	// ブルーム (b0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(RootParametersCombine[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(RootParametersCombine[1], descriptorRangeCombine[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	PSOFunction::SetRootParameter(RootParametersCombine[2], descriptorRangeCombine[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(posteffect_.rootSignature, RootParametersCombine, _countof(RootParametersCombine), staticSamplersBlur, _countof(staticSamplersBlur));
}

#pragma endregion












