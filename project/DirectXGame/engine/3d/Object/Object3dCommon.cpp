#include"Object3dCommon.h"

void Object3dCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;	// DX共通クラス

	// POS管理クラス
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// パイプライン生成
	CreateGraphicsPipeline();
}

void Object3dCommon::CreateGraphicsPipeline()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[4] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // ノーマルマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // スペキュラマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[3], 3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // AOマップ用

	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[11] = {};

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
	//トランスフォームデータ (b5) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[10], 5, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);


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

	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Object3D/Object3d.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Object3D/Object3d.PS.hlsl");



	// PSO生成

	// UV補間背面カリング
	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_SOLID_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	
	// UV補間なし背面カリング 
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_SOLID_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	//  UV補間背面カリングワイヤーフレーム
	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_WIREFRAME_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	
	//  UV補間なし背面カリングワイヤーフレーム
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);




	//  UV補間カリングなし
	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_SOLID_NONE, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	
	//  UV補間なしカリングなし
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_SOLID_NONE, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	//  UV補間カリングなしワイヤーフレーム
	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_WIREFRAME_NONE, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	
	//  UV補間なしカリングなしワイヤーフレーム
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
	, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);





	// ---- Transparent ----
	D3D12_BLEND_DESC blendAlpha = {};
	blendAlpha.AlphaToCoverageEnable = FALSE;
	blendAlpha.IndependentBlendEnable = FALSE;
	auto& rt = blendAlpha.RenderTarget[0];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;     // 直線アルファ
	rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	rt.BlendOp = D3D12_BLEND_OP_ADD;
	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_DEPTH_STENCIL_DESC depthAlpha = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthAlpha.DepthEnable = TRUE;
	depthAlpha.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // ← 書かない
	depthAlpha.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//  UV補間背面カリング
	psoManager_->CreatePso(PSOType::Transparent, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers)
		, D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID, blendAlpha, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}
