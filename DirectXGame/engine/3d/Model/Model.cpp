
// engine
#include"Model.h"
#include"ModelCommon.h"
//#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/Material/Material.h"
#include"DirectXGame/engine/base/Logger.h"
#include"DirectXGame/engine/base/StringUtility.h"

// C++
#include <iostream>
#include"format"
#include<vector>




std::string getLastPartOfPath(const std::string& path) {
	size_t pos = path.find_last_of("/\\"); if (pos == std::string::npos) {
		return path;
	} return path.substr(pos + 1);
}



#pragma region Initialize

void Model::Initialize(DirectXCommon* dxCommon, ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, const std::string& file, const Vector2 texScale)
{
	modelCommon_ = modelCommon;
	srvManager_ = modelCommon_->GetSrvManager();
	dxCommon_ = dxCommon;
	std::string dire = directorypath;

	if (file != "") {
		dire = directorypath + "/" + file;
	}


	modelData = LoadOdjFileAssimp(dire, filename);

	for (auto& material : modelData.material) {
		material->LoadTex();
	};

}

void Model::InitializeAnime(DirectXCommon* dxCommon, ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, const std::string& file)
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
	CreateModel::CreateMeshLine(modelData,modelData.mesh[0]->indices);

	if (modelData.isAmimetion) {
		;
		// アニメーション生成
		LoadModel::LoadAnimation(modelData, dire, filename);

		// スケルトン生成
		CreateModel::CreateSkeleton(modelData);

		// スキンクラスター生成
		CreateModel::CreateSkinCluster(modelData, modelCommon_);

		// 
		ApplyAnimation(modelData.skeleton, modelData.animation, modelData.animationTime);
	}

	// マテリアル読み込み
	for (auto& material : modelData.material) {
		material->LoadTex();
	};


}

#pragma endregion // 初期化

#pragma region Draw

void Model::Draw()
{
	for (auto& mesh : modelData.mesh)
	{
		modelData.material[mesh->meshIndex]->GetCommandListMaterial(0);

		modelData.material[mesh->meshIndex]->GetCommandListTexture(2, 7, 8);
	
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
		modelData.material[mesh->meshIndex]->GetCommandListMaterial(0);

		modelData.material[mesh->meshIndex]->GetCommandListTexture(2, 7, 8);

		mesh->GetCommandList(modelData.skinCluster.outputBufferView, modelData.skinCluster.influenceBufferView);

		// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
		commandList->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);

	}

	// 初期状態を UAV 用に遷移させる
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = modelData.skinCluster.outputVertexResource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	commandList->ResourceBarrier(1, &barrier);
}

#pragma endregion // 描画

#pragma region Load

ModelData Model::LoadOdjFileAssimp(const std::string& directoryPath, const std::string& filename, const Vector2 texScale) {
	////必要な変数の宣言とファイルを開く
	ModelData modelData;//構築するModelData

	Assimp::Importer importer;
	std::string filePach = directoryPath + "/" + filename;

	modelData.name = filePach;
	const aiScene* scene = importer.ReadFile(filePach.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); //メッシュがないのは対応しない
	if (!scene) {
		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
		return modelData;
	}

	// ノード読み込み
	modelData.rootNode = LoadModel::ReadNode(scene->mRootNode);

	// マテリアル読み込み
	LoadModel::LoadMaterial(scene, modelData, dxCommon_, directoryPath);
	
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals()); // 法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0)); //TexcoordがないMeshは今回は非対応

		std::unique_ptr<Mesh> pMesh = std::make_unique<Mesh>();
		pMesh->meshIndex = meshIndex;

		Vector3 min = { 100 };
		Vector3 max = { -100 };
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3); // 三角形のみサポート
			//Mesh::triangle
			Triangle triangle{ Vector3{},Vector3{},Vector3{} };
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;

				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x * texScale.x,texcoord.y * texScale.y };

				// aiProcess_MakeLeftHandedはz*=-1で、右手->左手に変換するので手動で対応
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				pMesh->vertices.push_back(vertex);
				pMesh->verticesline.push_back({ { vertex.position }, { 1,1,1,1 } });

				triangle.vertices[element] = vertex.position.xyz();

				min = Min(min, vertex.position.xyz());
				max = Max(max, vertex.position.xyz());
			}
			// 三角ポリゴン
			pMesh->triangle.push_back(std::move(triangle));
		}

		pMesh->SetMin(min);
		pMesh->SetMax(max);

		// インデックスを生成
		pMesh->GenerateIndices2(); // thisは省略可能

		pMesh->Initialize(dxCommon_);



		modelData.mesh.push_back(std::move(pMesh));
	}

	//
	return modelData;
}

ModelData Model::LoadOdjFileAssimpAmime(const std::string& directoryPath, const std::string& filename) {
	//必要な変数の宣言とファイルを開く
	ModelData modelData;//構築するModelData

	Assimp::Importer importer;
	std::string filePach = directoryPath + "/" + filename;

	modelData.name = filePach;

	const aiScene* scene = importer.ReadFile(filePach.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); //メッシュがないのは対応しない

	// ノード読み込み
	modelData.rootNode = LoadModel::ReadNode(scene->mRootNode);

	// メッシュ読み込み
	LoadModel::LoadMesh(scene, modelData, dxCommon_);
	
	// ボーン読み込み
	LoadModel::LoadBone(scene, modelData, dxCommon_);
	
	// マテリアル読み込み
	LoadModel::LoadMaterial(scene, modelData, dxCommon_, directoryPath);

	return modelData;
}
#pragma endregion // 読み込み系

