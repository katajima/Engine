// engine
#include"Model.h"
#include"ModelCommon.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/Material/Material.h"
#include"DirectXGame/engine/base/Logger.h"
#include"DirectXGame/engine/base/StringUtility.h"

// C++
#include <iostream>
#include"format"
#include<vector>

void Model::Initialize(DirectXCommon* dxCommon, ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, const std::string& file)
{
	modelCommon_ = modelCommon;
	dxCommon_ = dxCommon;
	srvManager_ = modelCommon_->GetSrvManager();

	std::string dire = directorypath;

	if (file != "") {
		dire = directorypath + "/" + file;
	}

	// モデル読み込み
	modelData = LoadOdjFileAssimpAmime(dire, filename);

	// メッシュ状にライン生成
	CreateModel::CreateMeshLine(modelData, modelData.mesh[0]->indices);


	
	// アニメーションがあるなら
	if (modelData.isAmimetion) {
		;
		// アニメーション生成
		LoadModel::LoadAnimation(modelData, dire, filename);

		// スケルトン生成
		CreateModel::CreateSkeleton(modelData);

		// スキンクラスター生成
		CreateModel::CreateSkinCluster(modelData, modelCommon_);

		auto it = modelData.animations.find(modelData.currentAnimName);
		if (it != modelData.animations.end()) {
			Animetion::ApplyAnimation(modelData.skeleton, it->second, modelData.animationTime);
		}
		// 
		//Animetion::ApplyAnimation(modelData.skeleton, modelData.animation, modelData.animationTime);
	}

	// マテリアル読み込み
	for (auto& mesh : modelData.mesh) {
		mesh->material->LoadTex();
	};


	std::string filenameT = " name : " + filename;
	timer_.LogTimeSec("All Load Time: ", filenameT);
}

void Model::Draw()
{
	for (auto& mesh : modelData.mesh)
	{
		mesh->material->GetCommandListMaterial(0);

		mesh->material->GetCommandListTexture(2, 7, 8);

		mesh->GetCommandList();

		// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
		modelCommon_->GetCommand()->GetList()->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);
	}
}

void Model::DrawSkinning()
{
	auto commandList = modelCommon_->GetCommand()->GetList();


	for (auto& mesh : modelData.mesh)
	{
		mesh->material->GetCommandListMaterial(0);

		mesh->material->GetCommandListTexture(2, 7, 8);

		mesh->GetCommandList(mesh->skinCluster->outputBufferView,mesh->GetVertexBufferView()/* modelData.skinCluster.influenceBufferView*/);

		// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
		commandList->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);


		// 初期状態を UAV 用に遷移させる
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mesh->skinCluster->outputVertexResource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		commandList->ResourceBarrier(1, &barrier);
	}
}

float Model::GetMaterialAlfa()
{
	float a = 1.000f;

	for (auto& mesh : modelData.mesh)
	{
		if (mesh->material->color.a < a) {
			a = mesh->material->color.a;
		}		
	}
	return a;
}

ModelData Model::LoadOdjFileAssimpAmime(const std::string& directoryPath, const std::string& filename) {
	//必要な変数の宣言とファイルを開く
	ModelData modelData;//構築するModelData
	
	timer_.StartTimer();

	Assimp::Importer importer;
	std::string filePach = directoryPath + "/" + filename;

	modelData.name = filePach;

	const aiScene* scene = importer.ReadFile(filePach.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); //メッシュがないのは対応しない

	// ノード読み込み
	modelData.rootNode = LoadModel::ReadNode(scene->mRootNode,modelData.meshOffsetMap);

	// メッシュ読み込み
	LoadModel::LoadMesh(scene, modelData, dxCommon_);
	// Assimp読み込みやメッシュ生成
	
	timer_.EndTimer();

	//std::string filenameT = " name : " + filename;

	//timer_.LogTimeSec("Model LoadMesh Time: ",filenameT);
	
	// ボーン読み込み
	LoadModel::LoadBone(scene, modelData, dxCommon_);
	
	// マテリアル読み込み
	LoadModel::LoadMaterial(scene, modelData, dxCommon_, directoryPath);

	timer_.EndTimer();

	return modelData;
}