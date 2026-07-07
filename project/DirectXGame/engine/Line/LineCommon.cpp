#include "LineCommon.h"
#include "combaseapi.h"

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/collider/Octree/Octree.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::LineCommon::Initialize(DirectXCommon* dxCommon)
{
	this->dxCommon = dxCommon;	// DX共通クラス

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());
	
	// パイプライン生成
	CreateGraphicsPipeline();

	


	// マテリアル
	materialResource = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//今回は赤を書き込んで見る //白
	*materialData = MaterialData({ 1.0f, 1.0f, 1.0f, 1.0f }); //RGBA


	// ビューリソース生成
	viewResource = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Matrix4x4));
	viewResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraWVP));

	*cameraWVP = MakeIdentity4x4();

	// 初期化
	lineMeshData_.Initialize(dxCommon);
	lineDebugMeshData_.Initialize(dxCommon);
}

void Engine::LineCommon::Update()
{
	lineMeshData_.Update();
#ifdef _DEBUG
	lineDebugMeshData_.Update();
#endif // _DEBUG


	if (camera && cameraWVP) {
		*cameraWVP = camera->GetViewProjectionMatrix();  // データをコピー
	}
}

void Engine::LineCommon::DrawCommonSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Engine::LineCommon::DrawCommonSetting2()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature2.Get());

	dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState2.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Engine::LineCommon::Draw()
{
	DrawCommonSetting();

	auto commandList = dxCommon->GetCommandList();

#ifdef _DEBUG
	// SRV (インスタンシングデータ) をルートパラメータ [0] に設定
	commandList->SetGraphicsRootConstantBufferView(1, materialResource->GetGPUVirtualAddress());

	// ビューデータ
	commandList->SetGraphicsRootConstantBufferView(0, viewResource->GetGPUVirtualAddress());


	lineDebugMeshData_.Draw();
#endif // _DEBUG

	// SRV (インスタンシングデータ) をルートパラメータ [0] に設定
	commandList->SetGraphicsRootConstantBufferView(1, materialResource->GetGPUVirtualAddress());

	// ビューデータ
	commandList->SetGraphicsRootConstantBufferView(0, viewResource->GetGPUVirtualAddress());

	lineMeshData_.Draw();
}

void Engine::LineCommon::LineClear()
{
	lineMeshData_.Clear();
	lineDebugMeshData_.Clear();
}

void Engine::LineCommon::CreateRootSignature()
{
	D3D12_ROOT_PARAMETER rootParameters[2] = {};  // 2つのパラメーターを使う

	// カメラデータ
	PSOFanction::SetRootParameter(rootParameters[0],0,D3D12_SHADER_VISIBILITY_VERTEX,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[1],0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);

	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters),nullptr,0);

	// カメラデータ
	PSOFanction::SetRootParameter(rootParameters[0],0,D3D12_SHADER_VISIBILITY_VERTEX,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[1],0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);

	psoManager_->SetRootSignature(rootSignature2, rootParameters, _countof(rootParameters),nullptr,0);


}

void Engine::LineCommon::CreateGraphicsPipeline()
{
	CreateRootSignature();


#pragma region BlendState

	// BlendState(ブレンドステート)の設定
	// 標準のアルファブレンド設定を生成する
	D3D12_BLEND_DESC blendDesc = PSOFanction::CreateAlphaBlendDesc();

#pragma endregion //BlendState(ブレンドステート)

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//
	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Line/Line.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Line/Line.PS.hlsl");


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = PSOFanction::CreateDepthStencilDesc();


	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState, blendDesc, depthStencilDesc,D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;

	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState2, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);



}
