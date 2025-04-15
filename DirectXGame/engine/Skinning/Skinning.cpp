#include "Skinning.h"

#include "DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/base/TextureManager.h"

void SkinningConmmon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(),dxCommon_->GetDXCCompiler());

	csPsoManager_ = std::make_unique <CSPSOManager>();
	csPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	CreateGraphicsPipeline();
}

void SkinningConmmon::DrawCommonSetting(PSOType type)
{
	switch (type)
	{
	case SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get());
		break;
	case SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get());
		break;
	case SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[4].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[5].Get());
		break;
	case SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[6].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[7].Get());
		break;
	default:
		break;
	}
}

void SkinningConmmon::DrawCompureSetting()
{

	dxCommon_->GetCommandList()->SetComputeRootSignature(computeRootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(computePipelineState.Get());

}


void SkinningConmmon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[5] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	psoManager_->SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // ノーマルマップ用
	psoManager_->SetDescriptorRenge(descriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // スペキュラマップ用
	psoManager_->SetDescriptorRenge(descriptorRange[3], 3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // AOマップ用
	psoManager_->SetDescriptorRenge(descriptorRange[4], 4, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // スキニング用

	

	

	// Roosignature(ルートシグネチャ)作成
	//ShaderとResorceをどのように関連付けるかを示したオブジェクト

	//Roosignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};

	descriptionSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。複数指定できるのではい
	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[11] = {};


	// マテリアルデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// トランスフォームデータ (b0) を頂点シェーダで使用する
	psoManager_->SetRootParameter(rootParameters[1], 0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// 方向性ライトデータ (b1) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[3], 1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// カメラデータ (b2) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[4], 2, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// ポイントライトデータ (b3) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[5], 3, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// スポットライトデータ (b4) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[6], 4, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[7], descriptorRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t2) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[8], descriptorRange[2], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t3) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[9], descriptorRange[3], D3D12_SHADER_VISIBILITY_PIXEL);
	//スキニング (t4) をバーテックスシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[10],descriptorRange[4], D3D12_SHADER_VISIBILITY_VERTEX);

	
	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature[0], rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));


	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature[1], rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
	

	


	D3D12_DESCRIPTOR_RANGE computeDescriptorRange[3] = {};
	csPsoManager_->SetDescriptorRenge(computeDescriptorRange[0], 0, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //Palette
	csPsoManager_->SetDescriptorRenge(computeDescriptorRange[1], 1, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //InputVertices
	csPsoManager_->SetDescriptorRenge(computeDescriptorRange[2], 2, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //Influence



	D3D12_ROOT_PARAMETER computeRootParameters[5] = {};
	csPsoManager_->SetRootParameter(computeRootParameters[0],0,D3D12_SHADER_VISIBILITY_ALL,D3D12_ROOT_PARAMETER_TYPE_CBV); // gSkinningInfomation

	csPsoManager_->SetRootParameter(computeRootParameters[1],computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);
	csPsoManager_->SetRootParameter(computeRootParameters[2],computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);
	csPsoManager_->SetRootParameter(computeRootParameters[3],computeDescriptorRange[2], D3D12_SHADER_VISIBILITY_ALL);
	csPsoManager_->SetRootParameter(computeRootParameters[4], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_UAV); // gSkinningInfomation


	csPsoManager_->SetRootSignature(computeRootSignature, computeRootParameters, _countof(computeRootParameters), staticSamplers, _countof(staticSamplers));



	

	
	


}

void SkinningConmmon::CreateGraphicsPipeline()
{
	CreateRootSignature();



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
	psoManager_->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	psoManager_->AddInputElementDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	psoManager_->AddInputElementDesc("WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
	psoManager_->AddInputElementDesc("INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1);


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Skining/SkinningObject3d.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Skining/SkinningObject3d.PS.hlsl";



	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[0], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[1], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[2], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[3], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[4], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[5], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[6], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[7], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	csPsoManager_->shderFile_.commpute.filePach = L"resources/shaders/Skining/SkinningObject3d.PS.hlsl";



}
