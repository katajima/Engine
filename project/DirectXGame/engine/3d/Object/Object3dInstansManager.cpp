#include "Object3dInstansManager.h"

#include"DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>
#include <DirectXGame/engine/Utility/ConvertUtility.h>

#pragma region Object3dInstansManager

void Object3dInstansManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;						// DX共通クラス
	srvManager_ = dxCommon_->GetSrvManager();	// SRV管理クラス

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(),
		dxCommon_->GetDXCCompiler());

	// モデル管理クラス
	modelManager_ = dxCommon_->GetModelManager();

	// パイプライン生成
	CreateGraphicsPipeline();
}


void Object3dInstansManager::Update() {

	// カメラあるなら
	if (camera_) {
		Matrix4x4 projectionMatrix = camera_->GetProjectionMatrix();
		Matrix4x4 viewMatrix = camera_->GetViewMatrix();
		Matrix4x4 viewprojection = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 local = Matrix4x4::Identity();
		// 全パーティクルグループに対する処理
		for (auto& val : objectGroups | std::views::values) // 各オブジェクトグループに対して
		{
			ObjectGroup& group = val;
			group.instanceCount = 0; // 描画すべきインスタンスのカウント
			group.mesh->material->GPUData();


			for (auto objectIterator = group.object.begin(); objectIterator != group
				.object.end();) {
				// 描画されていないもしくはオブジェクトがない場合はスキップ
				if (!objectIterator->is_ || !objectIterator->isDraw_) {
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
					group.instanceData[group.instanceCount].WVP = worldViewProjectionMatrix;
					group.instanceData[group.instanceCount].worldInverseTranspose = Transpose(Inverse(worldMatrix));

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

		entity3DManager_->GetLightManager()->DrawLight({ true,true,true });

		group.mesh->material->GetCommandListMaterial(0);

		group.mesh->material->GetCommandListTexture(2, 7, 8);

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

void Object3dInstansManager::Clear(const std::string& name) {
	auto it = objectGroups.find(name);
	if (it == objectGroups.end()) return;
	it->second.object.clear();
	it->second.idMap.clear();
	it->second.instanceCount = 0;
}

#pragma region Create

void Object3dInstansManager::CreateObject3dGroup(
	const std::string& name, const std::string& textureFilePath, Model* model,
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
	const std::string& name, const std::string& textureFilePath, ModelMesh* mesh,
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


void Object3dInstansManager::AddObject(const std::string& name,
	const std::string& texName,
	ObjectInstans&& object, int& id, MeshType type) {

	if (MeshType::kModel == type) {
		CreateObject3dGroup(name, texName, modelManager_->FindModel(name));
	}

	object.color = { 1, 1, 1, 1 };
	object.is_ = true;
	object.isDraw_ = true;

	if (texName.empty()) {
		object.texIndex = objectGroups[name].model->modelData.mesh[0]->material->tex_.diffuseIndex;
	}
	else {
		object.texIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(texName);
	}

	// ✅ ムーブ前にidを保存
	int objectId = object.id;

	// ✅ 完全に右辺値としてムーブされる
	objectGroups[name].object.emplace_back(std::move(object));

	// 追加した要素のインデックスを取得
	size_t index = objectGroups[name].object.size() - 1;

	// ID → インデックスで登録
	if (objectId == -1) {
		objectGroups[name].idMap[ConvertUtility::ToInt(index)] = index;
		id = ConvertUtility::ToInt(index);
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
			tile.Initialize(entity3DManager_,false);

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
			tile.is_ = true;
			tile.isDraw_ = true;
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

			AddObject(groupName, texPath, std::move(tile), id_,MeshType::kModel);
		}
	}
}

#pragma endregion // 生成or追加系



ObjectInstans* Object3dInstansManager::GetObjectById(
	const std::string& groupName, int id) {
	auto itGroup = objectGroups.find(groupName);
	if (itGroup == objectGroups.end()) {
		std::terminate(); // 即座にプログラム停止
		return nullptr;
	}

	auto& group = itGroup->second;
	auto  it = group.idMap.find(id);
	if (it == group.idMap.end()) {
		std::terminate(); // 即座にプログラム停止
		return nullptr;
	}

	size_t index = it->second;
	if (index >= group.object.size()) {
		std::terminate(); // 即座にプログラム停止
		return nullptr; // 念のため安全確認
	}

	return &group.object[index];
}

std::deque<ObjectInstans>& Object3dInstansManager::GetObjects(const std::string& groupName)
{
	auto itGroup = objectGroups.find(groupName);
	if (itGroup == objectGroups.end()) {
		static std::deque<ObjectInstans> empty; // 空のベクタを static で用意
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

#pragma endregion

#pragma region PSO


void Object3dInstansManager::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[5] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // ノーマルマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // スペキュラマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[3], 3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // AOマップ用
	PSOFanction::SetDescriptorRenge(descriptorRange[4],	0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);// インスタンシング用
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 4, UINT_MAX, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[11] = {};

	//CD3DX12_ROOT_PARAMETER 

	// マテリアル (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// インスタンシング(t1) をバーテックシェーダ使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[4],D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[2], descriptorRange[0],D3D12_SHADER_VISIBILITY_PIXEL);
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
	psoManager_->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

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


#pragma region ObjectInstans

void ObjectInstans::Initialize(Entity3DManager* entity3DManager, bool useCollider, Transform transfor) {
	transform.Initialize();
	transform.translate_ = transfor.translate;
	transform.rotate_ = transfor.rotate;
	transform.scale_ = transfor.scale;
	color = { 1,1,1,1 };
	useCollider_ = useCollider;
	isDelete_ = false;
	is_ = false;

	if (useCollider_) {
		colliderComponent_ = std::make_unique<ColliderComponent>();
		colliderComponent_->SetOwner(colliderComponent_.get());
		// ラインコモンをセット
		colliderComponent_->SetLineCommon(entity3DManager->Get3DLineCommon());
		// 登録（IDを取得したければ変数で受ける）
		colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());
		isColliderComponenyUpdate_ = true;
	}

	rigidBodyComponent_ = std::make_unique<RigidBodyComponent>();

}

void ObjectInstans::Update() {
	if (isDelete_) return;
	transform.Update();
	// コライダー
	if (GetColliderComponent()) {
		if (isColliderComponenyUpdate_) {
			colliderComponent_->UpdateAll(transform);
		}
	}

	// 物理
	if (GetRigidBodyComponent()) {
		rigidBodyComponent_->Integrate(MyGame::GameTime(), transform);
	}
}


ContactRecord& ObjectInstans::GetContactRecord() { return colliderComponent_->contactRecord_; };



#pragma endregion


