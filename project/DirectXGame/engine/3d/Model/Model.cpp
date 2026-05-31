// engine
#include"Model.h"
#include"ModelCommon.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/Material/Material.h"
#include"DirectXGame/engine/base/Logger.h"
#include"DirectXGame/engine/Utility/StringUtility.h"

// C++
#include <iostream>
#include"format"
#include<vector>

void Engine::Model::Initialize(DirectXCommon* dxCommon, ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, const std::string& file)
{
	Initialize(dxCommon, modelCommon, directorypath, filename, file, nullptr);
}

void Engine::Model::Initialize(DirectXCommon* dxCommon, ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, const std::string& file, std::mutex* gpuResourceMutex)
{
	this->modelCommon = modelCommon;					// モデル共通クラス
	this->dxCommon = dxCommon;						// DX共通クラス
	this->srvManager = modelCommon->GetSrvManager();// SRV管理クラス

	
	std::string dire = directorypath;

	if (file != "") {
		dire = directorypath + "/" + file;
	}

	// モデル読み込み
	modelData = LoadOdjFileAssimpAmime(dire, filename, gpuResourceMutex);

	// メッシュ状にライン生成
	CreateModel::CreateMeshLine(modelData, modelData.mesh[0]->GetIndices());



	// アニメーションがあるなら
	if (modelData.isAmimetion) {
		;
		// アニメーション生成
		LoadModel::LoadAnimation(modelData, dire, filename);

		// スケルトン生成
		CreateModel::CreateSkeleton(modelData);

		// スキンクラスター生成
		CreateModel::CreateSkinCluster(modelData, modelCommon, gpuResourceMutex);

		auto it = modelData.animations.find(modelData.currentAnimName);
		if (it != modelData.animations.end()) {
			AnimationFunction::ApplyAnimation(modelData.skeleton, it->second, 1.0f);
		}
	}

	// マテリアル読み込み
	for (auto& mesh : modelData.mesh) {
		if (gpuResourceMutex) {
			std::lock_guard<std::mutex> lock(*gpuResourceMutex);
			mesh->material->LoadTex();
		}
		else {
			mesh->material->LoadTex();
		}
	};


	std::string filenameT = " name : " + filename;
	timer_.LogTimeSec("All Load Time: ", filenameT);
}

void Engine::Model::Draw()
{
}

void Engine::Model::DrawSkinning(std::vector<MaterialInstance> matetials, std::vector<ConstantBuffer<Material::DataGPU>*> cbResourcePtr)
{
	auto commandList = modelCommon->GetCommand()->GetList();

	int i = 0;
	for (auto& mesh : modelData.mesh)
	{
		mesh->material->SetGPUMaterialInstance(matetials[i], cbResourcePtr[i]);

		mesh->material->GetCommandListTexture(2, 7, 8);

		mesh->GetCommandList(mesh->skinCluster->outputBufferView, mesh->GetVertexBufferView());

		// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
		commandList->DrawIndexedInstanced(UINT(mesh->GetIndices().size()), 1, 0, 0, 0);


		// 初期状態を UAV 用に遷移させる
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mesh->skinCluster->outputVertexResource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		commandList->ResourceBarrier(1, &barrier);
		i++;
	}
}

float Engine::Model::GetMaterialAlpha()
{
	float a = 1.000f;

	for (auto& mesh : modelData.mesh)
	{
		if (mesh->material->GetMaterialInstance().color.a < a || mesh->material->GetMaterialInstance().alpha_ < a) {
			a = mesh->material->GetMaterialInstance().color.a * mesh->material->GetMaterialInstance().alpha_;
		}
	}
	return a;
}

Engine::ModelData Engine::Model::LoadOdjFileAssimpAmime(const std::string& directoryPath, const std::string& filename, std::mutex* gpuResourceMutex) {
	//必要な変数の宣言とファイルを開く
	ModelData modelData;//構築するModelData

	timer_.StartTimer();

	Assimp::Importer importer;
	std::string filePach = directoryPath + "/" + filename;

	modelData.name = filePach;

	const aiScene* scene = importer.ReadFile(filePach.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices);

	assert(scene);

	assert(scene->HasMeshes()); //メッシュがないのは対応しない

	// ノード読み込み
	modelData.rootNode = LoadModel::ReadNode(scene->mRootNode, modelData.meshOffsetMap);

	// メッシュ読み込み
	LoadModel::LoadMesh(scene, modelData, dxCommon, gpuResourceMutex);
	// Assimp読み込みやメッシュ生成

	timer_.EndTimer();

	// ボーン読み込み
	LoadModel::LoadBone(scene, modelData, dxCommon);

	// マテリアル読み込み
	LoadModel::LoadMaterial(scene, modelData, dxCommon, directoryPath, gpuResourceMutex);

	timer_.EndTimer();

	return modelData;
}
