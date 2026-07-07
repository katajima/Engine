#include"SpriteCommon.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"


void Engine::SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	this->dxCommon = dxCommon;	// DX共通クラス

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());

	// パイプライン生成
	CreateGraphicsPipeline();
}

void Engine::SpriteCommon::CreateGraphicsPipeline()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);



	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// マテリアル
	PSOFanction::SetRootParameter(rootParameters[1], 0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);	// トランスフォーム
	PSOFanction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);				// テクスチャ用

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ
	D3D12_STATIC_SAMPLER_DESC staticSamplers2[1] = {};
	PSOFanction::SetSampler(staticSamplers2[0], 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ

#pragma region BlendState


	// 標準のアルファブレンド設定を生成する
	D3D12_BLEND_DESC blendDesc = PSOFanction::CreateAlphaBlendDesc();

#pragma endregion //BlendState(ブレンドステート)


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFanction::CreateDepthStencilDesc();

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Object2D/Object2D.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Object2D/Object2D.PS.hlsl");



	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_SOLID_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers),
		D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID, blendDesc, depthStencilDesc);
	
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_SOLID_BACK, rootParameters, _countof(rootParameters), staticSamplers2, _countof(staticSamplers2),
		D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID, blendDesc, depthStencilDesc);

	psoManager_->CreatePso(PSOType::UvInterpolation_MODE_WIREFRAME_BACK, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers),
		D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME, blendDesc, depthStencilDesc);
	
	psoManager_->CreatePso(PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK, rootParameters, _countof(rootParameters), staticSamplers2, _countof(staticSamplers2),
		D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME, blendDesc, depthStencilDesc);
}

