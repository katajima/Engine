#include"Object3dCommon.h"

#include "DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/base/TextureManager.h"



void Object3dCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(),dxCommon_->GetDXCCompiler());

	CreateGraphicsPipeline();
}

void Object3dCommon::DrawCommonSetting(PSOType type)
{
	switch (type)
	{
	case Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get());
		break;
	case Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get());
		break;
	case Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get());
		break;
	case Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get());
		break;
	case Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[4].Get());
		break;
	case Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[5].Get());
		break;
	case Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[6].Get());
		break;
	case Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[7].Get());
		break;
	default:
		break;
	}



	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Object3dCommon::AllDraw()
{

}

void Object3dCommon::CreateRootSignature()
{
	//HRESULT hr;
	

	D3D12_DESCRIPTOR_RANGE descriptorRange[4] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0],0,1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	psoManager_->SetDescriptorRenge(descriptorRange[1],1,1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // ノーマルマップ用
	psoManager_->SetDescriptorRenge(descriptorRange[2],2,1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // スペキュラマップ用
	psoManager_->SetDescriptorRenge(descriptorRange[3],3,1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // AOマップ用

	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[11] = {};

	// マテリアルデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0],0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// トランスフォームデータ (b0) を頂点シェーダで使用する
	psoManager_->SetRootParameter(rootParameters[1],0,D3D12_SHADER_VISIBILITY_VERTEX,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[2],descriptorRange[0],D3D12_SHADER_VISIBILITY_PIXEL);
	// 方向性ライトデータ (b1) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[3],1,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// カメラデータ (b2) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[4],2,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// ポイントライトデータ (b3) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[5],3,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// スポットライトデータ (b4) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[6],4,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[7], descriptorRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t2) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[8], descriptorRange[2], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t3) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[9], descriptorRange[3], D3D12_SHADER_VISIBILITY_PIXEL);
	//トランスフォームデータ (b5) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[10],5,D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature[0], rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));


	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature[1], rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
}

void Object3dCommon::CreateGraphicsPipeline()
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


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Object3D/Object3d.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Object3D/Object3d.PS.hlsl";


	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[0], blendDesc,depthStencilDesc,D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[1], blendDesc,depthStencilDesc,D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[2], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[3], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[4], blendDesc,depthStencilDesc,D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[5], blendDesc,depthStencilDesc,D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME);

	psoManager_->GraphicsPipelineState(rootSignature[0], graphicsPipelineState[6], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->GraphicsPipelineState(rootSignature[1], graphicsPipelineState[7], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

}
