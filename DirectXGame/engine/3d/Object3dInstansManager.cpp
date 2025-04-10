#include "Object3dInstansManager.h"

#include"DirectXGame/engine/MyGame/MyGame.h"

Object3dInstansManager* Object3dInstansManager::instance = nullptr;

Object3dInstansManager* Object3dInstansManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new Object3dInstansManager;
	}
	return instance;
}

void Object3dInstansManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void Object3dInstansManager::Initialize(DirectXCommon* dxCommon)
{
	this->camera_ = Object3dCommon::GetInstance()->GetDefaltCamera();

	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_);

	CreateGraphicsPipeline();
}


void Object3dInstansManager::Update()
{
   
	// 全パーティクルグループに対する処理
	for (auto& pair : objectGroups) // 各パーティクルグループに対して
	{
		ObjectGroup& group = pair.second;
		group.instanceCount = 0; // 描画すべきインスタンスのカウント

		Matrix4x4 projectionMatrix = camera_->GetProjectionMatrix();
		Matrix4x4 viewMatrix = camera_->GetViewMatrix();
		Matrix4x4 viewprojection = Multiply(viewMatrix, projectionMatrix);

		for (auto particleIterator = group.object.begin(); particleIterator != group.object.end(); )
		{
			if (group.instanceCount < kNumMaxInstance) {

				

				// ワールド行列を計算
				
				particleIterator->transform.Update();

				Matrix4x4 worldMatrix = particleIterator->transform.worldMat_;

				// ワールドビュー射影行列を合成
				Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewprojection);

				// インスタンシング用データに情報を書き込み
				group.instanceData[group.instanceCount].World = worldMatrix;
				group.instanceData[group.instanceCount].WVP = worldViewProjectionMatrix;
				group.instanceData[group.instanceCount].color = particleIterator->color;
				group.instanceData[group.instanceCount].textureIndex = particleIterator->texIndex;

				// インスタンス数をカウント
				++group.instanceCount;
			}

			++particleIterator;
		}
	}
}

void Object3dInstansManager::Draw()
{
	auto commandList = dxCommon_->GetCommandList();




	for (auto& pair : objectGroups) {
		ObjectGroup& group = pair.second;
		if (group.instanceCount == 0) {
			continue;
		}


		Object3dInstansManager::GetInstance()->DrawCommonSetting(group.rasteType, group.blendType);


		
		// インスタンシングデータのSRVのDescriptorTableを設定
		commandList->SetGraphicsRootDescriptorTable(1, group.instancingSrvHandleGPU);

		commandList->SetGraphicsRootDescriptorTable(2, SrvManager::GetInstance()->GetGPUDescriptorHandle());


		group.mesh->GetCommandList();

		// インスタンシング描画
		uint32_t instanceCount = (std::min)(group.instanceCount, kNumMaxInstance);
		commandList->DrawIndexedInstanced(static_cast<UINT>(group.mesh->indices.size()), instanceCount, 0, 0, 0);
	}

}



void Object3dInstansManager::DrawCommonSetting(RasterizerType rasteType, BlendType blendType)
{
	switch (blendType)
	{
	case Object3dInstansManager::BlendType::MODE_ADD:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get());
		}
		break;
	case Object3dInstansManager::BlendType::MODE_SUBTRACT:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get());
		}
		break;
	case Object3dInstansManager::BlendType::MODE_MUlLIPLY:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[4].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[5].Get());
		}
		break;
	default:
		break;
	}




	//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());


	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3dInstansManager::CreateObject3dGroup(const std::string name, const std::string textureFilePath, Model* model, RasterizerType rasteType, BlendType blendType)
{
	if (objectGroups.contains(name)) {
		return;
	}

	ObjectGroup& objectGroup = objectGroups[name];

	// 名前
	objectGroup.name = name;
	// モデル
	//particleGroup.model = model;
	objectGroup.mesh = model->modelData.mesh[0].get();
	objectGroup.mesh->UpdateVertexBuffer();
	objectGroup.mesh->UpdateIndexBuffer();

	objectGroup.model = model;

	// マテリアル
	//objectGroup.material = std::make_unique<Material>();
	//objectGroup.material->Initialize(dxCommon_);
	//objectGroup.material->tex_.diffuseFilePath = textureFilePath;
	//objectGroup.material->LoadTex();


	// GPUリソースの作成
	objectGroup.resource = dxCommon_->CreateBufferResource(sizeof(ObjectGPU) * kNumMaxInstance);
	// マッピング
	objectGroup.resource->Map(0, nullptr, reinterpret_cast<void**>(&objectGroup.instanceData));
	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		objectGroup.instanceData[i].World = MakeIdentity4x4();
		objectGroup.instanceData[i].WVP = MakeIdentity4x4();
		objectGroup.instanceData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// SRVの設定
	// SRVインデックスの取得と設定
	objectGroup.srvIndex = SrvManager::GetInstance()->Allocate();
	objectGroup.instancingSrvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(objectGroup.srvIndex);
	objectGroup.instancingSrvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(objectGroup.srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(objectGroup.srvIndex, objectGroup.resource.Get(), kNumMaxInstance, sizeof(ObjectGPU));



	// ブレンド
	objectGroup.blendType = blendType;

	// ラスタライザ
	objectGroup.rasteType = rasteType;
}

void Object3dInstansManager::CreateObject3dGroup(const std::string name, const std::string textureFilePath, Mesh* mesh, RasterizerType rasteType, BlendType blendType)
{
	if (objectGroups.contains(name)) {
		return;
	}

	ObjectGroup& objectGroup = objectGroups[name];

	// 名前
	objectGroup.name = name;
	// モデル
	//particleGroup.model = model;
	objectGroup.mesh = mesh;
	objectGroup.mesh->UpdateVertexBuffer();
	objectGroup.mesh->UpdateIndexBuffer();

	
	// マテリアル
	//objectGroup.material = std::make_unique<Material>();
	//objectGroup.material->Initialize(dxCommon_);
	//objectGroup.material->tex_.diffuseFilePath = textureFilePath;
	//objectGroup.material->LoadTex();


	// GPUリソースの作成
	objectGroup.resource = dxCommon_->CreateBufferResource(sizeof(ObjectGPU) * kNumMaxInstance);
	// マッピング
	objectGroup.resource->Map(0, nullptr, reinterpret_cast<void**>(&objectGroup.instanceData));
	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		objectGroup.instanceData[i].World = MakeIdentity4x4();
		objectGroup.instanceData[i].WVP = MakeIdentity4x4();
		objectGroup.instanceData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// SRVの設定
	// SRVインデックスの取得と設定
	objectGroup.srvIndex = SrvManager::GetInstance()->Allocate();
	objectGroup.instancingSrvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(objectGroup.srvIndex);
	objectGroup.instancingSrvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(objectGroup.srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(objectGroup.srvIndex, objectGroup.resource.Get(), kNumMaxInstance, sizeof(ObjectGPU));



	// ブレンド
	objectGroup.blendType = blendType;

	// ラスタライザ
	objectGroup.rasteType = rasteType;
}


void Object3dInstansManager::AddObject(const std::string name, const std::string texName, ObjectInstans& object, MeshType type)
{
	if (MeshType::kModel == type) {
		CreateObject3dGroup(name, texName, ModelManager::GetInstance()->FindModel(name));

	}
	else {
		//CreateObject3dGroup(name, texName);
	}
	
	//Object object;
	object.color = { 1,1,1,1 };
	object.is = true;
	

	if (texName == "") {
		object.texIndex = objectGroups[name].model->modelData.material[0]->tex_.diffuseIndex;
	}
	else {
		object.texIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(texName);
	}
	

	objectGroups[name].object.push_back(object);
}

void Object3dInstansManager::Clear(const std::string name)
{
	objectGroups[name].object.clear();
}

#pragma region PSO


void Object3dInstansManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 1, UINT_MAX, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	psoManager_->SetDescriptorRenge(descriptorRange[1], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // インスタンシング用


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	//CD3DX12_ROOT_PARAMETER 

	// マテリアル (b4) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// インスタンシング(t1) をバーテックシェーダ使用する
	psoManager_->SetRootParameter(rootParameters[1], descriptorRange[1], D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// 方向性ライトデータ (b1) をピクセルシェーダで使用する
	//psoManager_->SetRootParameter(rootParameters[3], 1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// カメラデータ (b2) をピクセルシェーダで使用する
	//psoManager_->SetRootParameter(rootParameters[4], 2, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// ポイントライトデータ (b3) をピクセルシェーダで使用する
	///psoManager_->SetRootParameter(rootParameters[5], 3, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// スポットライトデータ (b4) をピクセルシェーダで使用する
	//psoManager_->SetRootParameter(rootParameters[6], 4, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

}

void Object3dInstansManager::CreateGraphicsPipeline()
{
	CreateRootSignature();

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 透明オブジェクトの場合はデプス書き込みを無効化
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Object3D/Object3dInstans.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Object3D/Object3dInstans.PS.hlsl";


	BlendAdd();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[0], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[1], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendSubtract();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[2], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[3], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendMuliply();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[4], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[5], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}

#pragma region Blend

void Object3dInstansManager::BlendAdd()
{
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Object3dInstansManager::BlendSubtract()
{
	// 減算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RGB の減算ブレンド
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;      // ソースの影響度
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // デスティネーションの影響度
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;     // 減算

	// アルファブレンドの設定（通常の設定）
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Object3dInstansManager::BlendMuliply()
{

	// 加算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 通常のカラー（RGB）加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;      // ソースカラーをそのまま
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;     // デスティネーションカラーをそのまま
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;    // 加算（SrcColor + DestColor）
	// アルファブレンドの設定
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;


}

#pragma endregion // ブレンド

#pragma endregion // パイプライン関係



void ObjectInstans::Initialize(Transform transfor)
{

	transform.Initialize();
	transform.translate_ = transfor.translate;
	transform.rotate_ = transfor.rotate;
	transform.scale_ = transfor.scale;



}

void ObjectInstans::Update()
{



}




