#include "Skinning.h"


void Engine::SkinningConmmon::Initialize(DirectXCommon* dxCommon)
{
	this->dxCommon = dxCommon;

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(),dxCommon->GetDXCCompiler());

	// コンピュート用PSOマネージャー初期化
	csPsoManager_ = std::make_unique <CSPSOManager>();
	csPsoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());

	// パイプライン生成
	CreateGraphicsPipeline();
	CreateShadowMapPipeline();
}

void Engine::SkinningConmmon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE computeDescriptorRange[4] = {};
	PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //Palette
	PSOFanction::SetDescriptorRenge(computeDescriptorRange[1], 1, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //InputVertices
	PSOFanction::SetDescriptorRenge(computeDescriptorRange[2], 2, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //Influence
	PSOFanction::SetDescriptorRenge(computeDescriptorRange[3], 0, 1,D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //OutputVertices



	D3D12_ROOT_PARAMETER computeRootParameters[5] = {};
	PSOFanction::SetRootParameter(computeRootParameters[0],0, D3D12_SHADER_VISIBILITY_ALL,D3D12_ROOT_PARAMETER_TYPE_CBV); // gSkinningInfomation

	PSOFanction::SetRootParameter(computeRootParameters[1], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL); //Palette
	PSOFanction::SetRootParameter(computeRootParameters[2], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL); //InputVertices
	PSOFanction::SetRootParameter(computeRootParameters[3], computeDescriptorRange[2], D3D12_SHADER_VISIBILITY_ALL); //Influence
	PSOFanction::SetRootParameter(computeRootParameters[4], computeDescriptorRange[3], D3D12_SHADER_VISIBILITY_ALL); //OutputVertices
	

	csPsoManager_->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
}

void Engine::SkinningConmmon::CreateGraphicsPipeline()
{
	CreateRootSignature();

	D3D12_DESCRIPTOR_RANGE descriptorRange[5] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // ノーマルマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // スペキュラマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[3], 3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // AOマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[4], 4, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // シャドウマップ用

	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[12] = {};


	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// トランスフォームデータ (b0) を頂点シェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[1], 0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// 方向性ライトデータ (b1) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[3], 1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// カメラデータ (b2) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[4], 2, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// ポイントライトデータ (b3) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[5], 3, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// スポットライトデータ (b4) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[6], 4, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[7], descriptorRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t2) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[8], descriptorRange[2], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t3) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[9], descriptorRange[3], D3D12_SHADER_VISIBILITY_PIXEL);
	// シャドウマップ (t4) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[10], descriptorRange[4], D3D12_SHADER_VISIBILITY_PIXEL);
	// シャドウ行列などのデータ (b6) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[11], 6, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers2[2] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ
	PSOFanction::SetSampler(staticSamplers2[0], 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ
	staticSamplers[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplers[1].MipLODBias = 0.0f;
	staticSamplers[1].MaxAnisotropy = 1;
	// 現在深度-bias <= 保存済み深度ならライトが届いている。D3Dの深度シャドウではLESS_EQUALを使う。
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	staticSamplers[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	staticSamplers[1].MinLOD = 0.0f;
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[1].ShaderRegister = 1;
	staticSamplers[1].RegisterSpace = 0;
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplers2[1] = staticSamplers[1];

#pragma region BlendState

	// BlendState(ブレンドステート)の設定
	// 標準のアルファブレンド設定を生成する
	D3D12_BLEND_DESC blendDesc = PSOFanction::CreateAlphaBlendDesc();

#pragma endregion //BlendState(ブレンドステート)

	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFanction::CreateDepthStencilDesc();


	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	psoManager_->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
	psoManager_->AddInputElementDesc("INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1);


	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Skining/NewSkinning.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Skining/SkinningObject3d.PS.hlsl");



	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_SOLID_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID,blendDesc,depthStencilDesc);
	
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_SOLID_BACK, rootParameters, _countof(rootParameters), staticSamplers2, _countof(staticSamplers2)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID,blendDesc,depthStencilDesc);

	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_WIREFRAME_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME,blendDesc,depthStencilDesc);
	
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK, rootParameters, _countof(rootParameters), staticSamplers2, _countof(staticSamplers2)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME,blendDesc,depthStencilDesc);

	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_SOLID_NONE, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID,blendDesc,depthStencilDesc);
	
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_SOLID_NONE, rootParameters, _countof(rootParameters), staticSamplers2, _countof(staticSamplers2)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID,blendDesc,depthStencilDesc);

	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_WIREFRAME_NONE, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME,blendDesc,depthStencilDesc);
	
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE, rootParameters, _countof(rootParameters), staticSamplers2, _countof(staticSamplers2)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME,blendDesc,depthStencilDesc);


	csPsoManager_->SetShaderFileName(L"resources/shaders/Skining/Skinning.CS.hlsl");
	
	csPsoManager_->ComputePipelineState();
}

void Engine::SkinningConmmon::CreateShadowMapPipeline()
{
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);
	PSOFanction::SetRootParameter(rootParameters[1], 6, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);

	// シャドウマップはカラーバッファへ書き込まない
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;

	// シャドウマップでも標準の深度比較と書き込み設定を利用する
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFanction::CreateDepthStencilDesc();

	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Object3D/ShadowMap.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"");
	psoManager_->SetRenderTargetFormats(0, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_D32_FLOAT);
	psoManager_->CreatePso(PSOType::ShadowMap, rootParameters, _countof(rootParameters), nullptr, 0,
		D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Skining/NewSkinning.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Skining/SkinningObject3d.PS.hlsl");
	psoManager_->SetRenderTargetFormats(1, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_D24_UNORM_S8_UINT);
}
