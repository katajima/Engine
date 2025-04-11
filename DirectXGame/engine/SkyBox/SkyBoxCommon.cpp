#include "SkyBoxCommon.h"

#include "DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/base/TextureManager.h"

SkyBoxCommon* SkyBoxCommon::instance = nullptr;

SkyBoxCommon* SkyBoxCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new SkyBoxCommon;
	}
	return instance;
}

void SkyBoxCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	CreateGraphicsPipeline();


	transfomation = std::make_unique<Transfomation>();
	transfomation->Initialize(dxCommon_);

	material = std::make_unique<Material>();
	material->Initialize(dxCommon_);
	material->tex_.diffuseFilePath = "resources/Texture/rostock_laage_airport_4k.dds";
	material->color = { 1,1,1,1};
	material->enableLighting_ = false;
	


	mesh_ = std::make_unique<Mesh>();
	
	Vector3 size = Vector3{10,10,10} * 100;

	// 各面の頂点座標 (1つの面に4頂点)
	Vector4 positions[][4] = {
		// 前面
		{{ size.x,  size.y,  size.z, 1.0f}, {-size.x,  size.y,  size.z, 1.0f}, { size.x, -size.y,  size.z, 1.0f}, {-size.x, -size.y,  size.z, 1.0f}},
		// 背面
		{{ size.x,  size.y, -size.z, 1.0f}, { size.x, -size.y, -size.z, 1.0f}, {-size.x,  size.y, -size.z, 1.0f}, {-size.x, -size.y, -size.z, 1.0f}},
		// 上面
		{{ size.x,  size.y, -size.z, 1.0f}, {-size.x,  size.y, -size.z, 1.0f}, { size.x,  size.y,  size.z, 1.0f}, {-size.x,  size.y,  size.z, 1.0f}},
		// 底面
		{{ size.x, -size.y,  size.z, 1.0f}, {-size.x, -size.y,  size.z, 1.0f}, { size.x, -size.y, -size.z, 1.0f}, {-size.x, -size.y, -size.z, 1.0f}},
		// 右側面
		{{ size.x,  size.y, -size.z, 1.0f}, { size.x,  size.y,  size.z, 1.0f}, { size.x, -size.y, -size.z, 1.0f}, { size.x, -size.y,  size.z, 1.0f}},
		// 左側面
		{{-size.x,  size.y,  size.z, 1.0f}, {-size.x,  size.y, -size.z, 1.0f}, {-size.x, -size.y,  size.z, 1.0f}, {-size.x, -size.y, -size.z, 1.0f}}
	};

	// 各面のインデックスオフセット
	int vertexOffset = 0;

	for (int i = 0; i < 6; ++i) { // 6面
		// 頂点データを追加
		for (int j = 0; j < 4; ++j) { // 各面の4頂点
			mesh_->verticesskyBox.push_back({
				positions[i][j],      // 座標
				});
		}

		// インデックスデータを追加 (2つの三角形)
		mesh_->indices.push_back(vertexOffset + 0);
		mesh_->indices.push_back(vertexOffset + 2);
		mesh_->indices.push_back(vertexOffset + 1);

		mesh_->indices.push_back(vertexOffset + 2);
		mesh_->indices.push_back(vertexOffset + 3);
		mesh_->indices.push_back(vertexOffset + 1);

		vertexOffset += 4; // 次の面に移動
	}

	mesh_->InitializeSkyBox(dxCommon_->GetModelManager()->GetModelCommon());


	worldtransform_.Initialize();
	worldtransform_.scale_ = { 10,10,10 };
}

void SkyBoxCommon::Update()
{
	worldtransform_.Update();

	transfomation->Update(camara_,worldtransform_.worldMat_);

}

void SkyBoxCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void SkyBoxCommon::DrawCommonSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mesh_->GetCommandList();



	material->GetCommandListMaterial(0);

	material->GetCommandListTexture(2, 7, 8);

	transfomation->GetCommandList(1);

	// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(mesh_->indices.size()), 1, 0, 0, 0);

}

void SkyBoxCommon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0],0,1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	// マテリアルデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0],0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);

	// トランスフォームデータ (b0) を頂点シェーダで使用する
	psoManager_->SetRootParameter(rootParameters[1],0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);

	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	psoManager_->SetSampler(staticSamplers[0],0 ,D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);

	
	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature,rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
	

}

void SkyBoxCommon::CreateGraphicsPipeline()
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

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK,D3D12_FILL_MODE_SOLID);

	
	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/SkyBox/SkyBox.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/SkyBox/SkyBox.PS.hlsl";


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	psoManager_->GraphicsPipelineState(rootSignature,graphicsPipelineState,blendDesc, depthStencilDesc);
	
}
