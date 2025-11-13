#include "Skinning.h"


void SkinningConmmon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(),dxCommon_->GetDXCCompiler());

	// コンピュート用PSOマネージャー初期化
	csPsoManager_ = std::make_unique <CSPSOManager>();
	csPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// パイプライン生成
	CreateGraphicsPipeline();
}

void SkinningConmmon::CreateRootSignature()
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

void SkinningConmmon::CreateGraphicsPipeline()
{
	CreateRootSignature();

	D3D12_DESCRIPTOR_RANGE descriptorRange[4] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // ノーマルマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // スペキュラマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[3], 3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // AOマップ用

	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[10] = {};


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


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers2[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ
	PSOFanction::SetSampler(staticSamplers2[0], 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ

#pragma region BlendState

	// BlendState(ブレンドステート)の設定
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
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	psoManager_->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
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
