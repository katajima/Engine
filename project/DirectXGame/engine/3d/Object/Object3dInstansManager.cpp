#include "Object3dInstansManager.h"

#include"DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

void Object3dInstansManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	srvManager_ = dxCommon_->GetSrvManager();
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(),
		dxCommon_->GetDXCCompiler());

	modelManager_ = dxCommon_->GetModelManager();
	CreateGraphicsPipeline();
}


void Object3dInstansManager::Update() {
	if (camera_) {
		Matrix4x4 projectionMatrix = camera_->GetProjectionMatrix();
		Matrix4x4 viewMatrix = camera_->GetViewMatrix();
		Matrix4x4 viewprojection = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 local = Matrix4x4::Identity();
		// 全パーティクルグループに対する処理
		for (auto& val : objectGroups | std::views::values) // 各パーティクルグループに対して
		{
			ObjectGroup& group = val;
			group.instanceCount = 0; // 描画すべきインスタンスのカウント
			group.mesh->material->GPUData();


			for (auto objectIterator = group.object.begin(); objectIterator != group
				.object.end();) {
				if (!objectIterator->is) {
					++objectIterator;
					continue;
				}



				if (group.instanceCount < kNumMaxInstance) {
					// ワールド行列を計算
					objectIterator->Update();

					Matrix4x4 worldMatrix = objectIterator->transform.worldMat_;

					// ワールドビュー射影行列を合成
					Matrix4x4 worldViewProjectionMatrix = Multiply(
						worldMatrix, viewprojection);

					// インスタンシング用データに情報を書き込み
					group.instanceData[group.instanceCount].World = worldMatrix;
					group.instanceData[group.instanceCount].WVP =
						worldViewProjectionMatrix;
					group.instanceData[group.instanceCount].color = objectIterator->
						color;
					group.instanceData[group.instanceCount].textureIndex =
						objectIterator->texIndex;

					// インスタンス数をカウント
					++group.instanceCount;
				}

				++objectIterator;
			}
		}
	}
}

void Object3dInstansManager::Draw() {
	auto commandList = dxCommon_->GetCommandList();

	for (auto& pair : objectGroups) {
		ObjectGroup& group = pair.second;
		if (group.instanceCount == 0) {
			continue;
		}


		DrawCommonSetting(group.rasteType, group.blendType);

		entity3DManager_->GetLightManager()->DrawLight({ true,false,false });

		group.mesh->material->GetCommandListMaterial(0);

		camera_->GetCommandList(4);

		// インスタンシングデータのSRVのDescriptorTableを設定
		commandList->SetGraphicsRootDescriptorTable(
			1, group.instancingSrvHandleGPU);

		commandList->SetGraphicsRootDescriptorTable(
			2, srvManager_->GetGPUDescriptorHandle());


		group.mesh->GetCommandList();

		// インスタンシング描画
		uint32_t instanceCount = (std::min)(group.instanceCount,
			kNumMaxInstance);
		commandList->DrawIndexedInstanced(
			static_cast<UINT>(group.mesh->indices.size()), instanceCount, 0, 0,
			0);
	}
}


void Object3dInstansManager::DrawCommonSetting(RasterizerType rasteType,
	BlendType      blendType) {
	switch (blendType) {
	case BlendType::MODE_ADD:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(
				graphicsPipelineState[0].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(
				graphicsPipelineState[1].Get());
		}
		break;
	case BlendType::MODE_SUBTRACT:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(
				graphicsPipelineState[2].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(
				graphicsPipelineState[3].Get());
		}
		break;
	case BlendType::MODE_MUlLIPLY:
		if (rasteType == RasterizerType::MODE_SOLID_BACK) {
			dxCommon_->GetCommandList()->SetPipelineState(
				graphicsPipelineState[4].Get());
		}
		else {
			dxCommon_->GetCommandList()->SetPipelineState(
				graphicsPipelineState[5].Get());
		}
		break;
	default:
		break;
	}


	//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());


	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3dInstansManager::Clear(const std::string name) {
	auto it = objectGroups.find(name);
	if (it == objectGroups.end()) return;
	it->second.object.clear();
	it->second.idMap.clear();
	it->second.instanceCount = 0;
}

#pragma region Create

void Object3dInstansManager::CreateObject3dGroup(
	const std::string name, const std::string textureFilePath, Model* model,
	RasterizerType    rasteType, BlendType    blendType) {
	if (objectGroups.contains(name)) {
		return;
	}

	ObjectGroup& objectGroup = objectGroups[name];

	// 名前
	objectGroup.name = name;
	// モデル
	objectGroup.mesh = model->modelData.mesh[0].get();
	objectGroup.mesh->UpdateVertexBuffer();
	objectGroup.mesh->UpdateIndexBuffer();

	objectGroup.model = model;

	// GPUリソースの作成
	objectGroup.resource = dxCommon_->GetDXGIDevice()->CreateBufferResource(
		sizeof(ObjectGPU) * kNumMaxInstance);
	// マッピング
	objectGroup.resource->Map(0, nullptr,
		reinterpret_cast<void**>(&objectGroup.
			instanceData));
	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		objectGroup.instanceData[i].World = MakeIdentity4x4();
		objectGroup.instanceData[i].WVP = MakeIdentity4x4();
		objectGroup.instanceData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// SRVの設定
	// SRVインデックスの取得と設定
	objectGroup.srvIndex = srvManager_->Allocate();
	objectGroup.instancingSrvHandleCPU = srvManager_->GetCPUDescriptorHandle(
		objectGroup.srvIndex);
	objectGroup.instancingSrvHandleGPU = srvManager_->GetGPUDescriptorHandle(
		objectGroup.srvIndex);
	srvManager_->CreateSRVforStructuredBuffer(objectGroup.srvIndex,
		objectGroup.resource.Get(),
		kNumMaxInstance,
		sizeof(ObjectGPU));
	// ブレンド
	objectGroup.blendType = blendType;

	// ラスタライザ
	objectGroup.rasteType = rasteType;
}

void Object3dInstansManager::CreateObject3dGroup(
	const std::string name, const std::string textureFilePath, ModelMesh* mesh,
	RasterizerType    rasteType, BlendType    blendType) {
	if (objectGroups.contains(name)) {
		return;
	}

	ObjectGroup& objectGroup = objectGroups[name];

	// 名前
	objectGroup.name = name;
	// モデル
	objectGroup.mesh = mesh;
	objectGroup.mesh->UpdateVertexBuffer();
	objectGroup.mesh->UpdateIndexBuffer();


	// GPUリソースの作成
	objectGroup.resource = dxCommon_->GetDXGIDevice()->CreateBufferResource(
		sizeof(ObjectGPU) * kNumMaxInstance);
	// マッピング
	objectGroup.resource->Map(0, nullptr,
		reinterpret_cast<void**>(&objectGroup.
			instanceData));
	// 初期化
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		objectGroup.instanceData[i].World = MakeIdentity4x4();
		objectGroup.instanceData[i].WVP = MakeIdentity4x4();
		objectGroup.instanceData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// SRVの設定
	// SRVインデックスの取得と設定
	objectGroup.srvIndex = srvManager_->Allocate();
	objectGroup.instancingSrvHandleCPU = srvManager_->GetCPUDescriptorHandle(
		objectGroup.srvIndex);
	objectGroup.instancingSrvHandleGPU = srvManager_->GetGPUDescriptorHandle(
		objectGroup.srvIndex);
	srvManager_->CreateSRVforStructuredBuffer(objectGroup.srvIndex,
		objectGroup.resource.Get(),
		kNumMaxInstance,
		sizeof(ObjectGPU));
	// ブレンド
	objectGroup.blendType = blendType;

	// ラスタライザ
	objectGroup.rasteType = rasteType;
}


void Object3dInstansManager::AddObject(const std::string name,
	const std::string texName,
	ObjectInstans& object, MeshType type) {
	if (MeshType::kModel == type) {
		CreateObject3dGroup(name, texName, modelManager_->FindModel(name));
	}


	object.color = { 1, 1, 1, 1 };
	object.is = true;


	if (texName == "") {
		object.texIndex = objectGroups[name].model->modelData.mesh[0]->material
			->tex_.diffuseIndex;
	}
	else {
		object.texIndex = dxCommon_->GetTextureManager()->
			GetTextureIndexByFilePath(texName);
	}


	objectGroups[name].object.push_back(object);

	// 追加した要素のインデックスを取得
	size_t index = objectGroups[name].object.size() - 1;

	// ID → インデックスで登録
	if (object.id != -1) {
		objectGroups[name].idMap[object.id] = index;
	}
}

void Object3dInstansManager::CreateTileMap(const std::string& groupName,
	const std::string& textureFilePath,
	Model* tileModel,
	int mapWidth, int mapHeight,
	Vector3 tileSize,
	Vector2 tileInterval,
	const std::vector<int>& tileIndices,
	const std::vector<MapId>& mapIds,
	MapAxis axis) {
	// グループを作成
	CreateObject3dGroup(groupName, textureFilePath, tileModel);

	// タイル配置を生成
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			int idx = y * mapWidth + x;
			int tileType = tileIndices[idx];
			if (tileType <= 0) continue; // 0以下 = 空

			ObjectInstans tile;
			tile.Initialize();

			Vector3 pos{};

			// --- 配置方向を MapAxis で切り替え ---
			switch (axis) {
			case MapAxis::XY:
				// Z = 0、XY 平面に敷く
				pos = { x * tileInterval.x, y * tileInterval.y, 0.0f };
				break;

			case MapAxis::ZX:
				// Y = 0、XZ 平面に敷く（Z が縦、X が横）
				pos = { x * tileInterval.x, 0.0f, y * tileInterval.y };
				break;

			case MapAxis::YZ:
				// X = 0、YZ 平面に敷く（Y が縦、Z が横）
				pos = { 0.0f, y * tileInterval.y, x * tileInterval.x };
				break;
			}

			tile.transform.translate_ = pos;
			tile.transform.scale_ = tileSize;

			// テクスチャインデックス設定
			tile.texIndex = tileType; // spriteシートの場合はインデックス切替で管理可能
			tile.is = true;
			// タイルID（オプション、必要なら）
			tile.id = y * mapWidth + x;
			// グループにタイルを追加


			std::string texPath = textureFilePath;
			if (mapIds.size() > 0) {
				for (const auto& id : mapIds) {
					if (id.id == tileType) {
						texPath = id.tex;
						break;
					}
				}
			}

			AddObject(groupName, texPath, tile, MeshType::kModel);
		}
	}
}

#pragma endregion // 生成or追加系

void Object3dInstansManager::MoveTile(const std::string& groupName, int tileId,
	const Vector3& newPos) {
	auto& group = objectGroups[groupName];
	for (auto& obj : group.object) {
		if (obj.is && obj.id == tileId) {
			obj.transform.translate_ = newPos;
			return;
		}
	}
}

ObjectInstans* Object3dInstansManager::GetObjectById(
	const std::string& groupName, int id) {
	auto itGroup = objectGroups.find(groupName);
	if (itGroup == objectGroups.end()) {
		return nullptr;
	}

	auto& group = itGroup->second;
	auto  it = group.idMap.find(id);
	if (it == group.idMap.end()) {
		return nullptr;
	}

	size_t index = it->second;
	if (index >= group.object.size()) {
		return nullptr; // 念のため安全確認
	}

	return &group.object[index];
}

std::vector<ObjectInstans>& Object3dInstansManager::GetObjects(const std::string& groupName)
{
	auto itGroup = objectGroups.find(groupName);
	if (itGroup == objectGroups.end()) {
		static std::vector<ObjectInstans> empty; // 空のベクタを static で用意
		return empty; // 空参照を返す
	}

	return itGroup->second.object; // コピーして返す
}

Object3dInstansManager::ObjectGroup& Object3dInstansManager::GetObjectGroup(const std::string& groupName)
{
	auto itGroup = objectGroups.find(groupName);
	if (itGroup == objectGroups.end())
	{
		throw std::runtime_error("Object group not found: " + groupName);
	}

	return itGroup->second;
}

#pragma region PSO


void Object3dInstansManager::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 1, UINT_MAX,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 0, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV);// インスタンシング用


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[5] = {};

	//CD3DX12_ROOT_PARAMETER 

	// マテリアル (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// インスタンシング(t1) をバーテックシェーダ使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[1],D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[2], descriptorRange[0],D3D12_SHADER_VISIBILITY_PIXEL);
	// 方向性ライトデータ (b1) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[3], 1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// カメラデータ (b2) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[4], 2, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);



	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0,D3D12_FILTER_MIN_MAG_MIP_LINEAR,D3D12_SHADER_VISIBILITY_PIXEL); // バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters,
		_countof(rootParameters), staticSamplers,
		_countof(staticSamplers));
}

void Object3dInstansManager::CreateGraphicsPipeline() {
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
	psoManager_->AddInputElementDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->SetShaderFileName(ShaderFileName::VS,
		L"resources/shaders/Object3D/Object3dInstans.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS,
		L"resources/shaders/Object3D/Object3dInstans.PS.hlsl");


	BlendAdd();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[0],
		blendDesc, depthStencilDesc,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[1],
		blendDesc, depthStencilDesc,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	BlendSubtract();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[2],
		blendDesc, depthStencilDesc,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[3],
		blendDesc, depthStencilDesc,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendMuliply();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[4],
		blendDesc, depthStencilDesc,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[5],
		blendDesc, depthStencilDesc,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}

#pragma region Blend

void Object3dInstansManager::BlendAdd() {
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Object3dInstansManager::BlendSubtract() {
	// 減算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RGB の減算ブレンド
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースの影響度
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // デスティネーションの影響度
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT; // 減算

	// アルファブレンドの設定（通常の設定）
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Object3dInstansManager::BlendMuliply() {
	// 加算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	// 通常のカラー（RGB）加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; // ソースカラーをそのまま
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // デスティネーションカラーをそのまま
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	// 加算（SrcColor + DestColor）
	// アルファブレンドの設定
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

#pragma endregion // ブレンド

#pragma endregion // パイプライン関係


void ObjectInstans::Initialize(Transform transfor) {
	transform.Initialize();
	transform.translate_ = transfor.translate;
	transform.rotate_ = transfor.rotate;
	transform.scale_ = transfor.scale;
	color = { 1,1,1,1 };
}

void ObjectInstans::Update() {
	transform.Update();
}
