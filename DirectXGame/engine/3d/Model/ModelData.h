#pragma once

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/Animation/Animation.h"
#include"DirectXGame/engine/Mesh/Mesh.h"
#include"DirectXGame/engine/struct/Material.h"
#include"DirectXGame/engine/Material/Material.h"

// C++
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include<span>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct SkinningSRVUAV {
	uint32_t wellSrvIndex; // Well
	uint32_t inputVerticesIndex; // 入力頂点
	uint32_t influencesIndex; // 入力インフルエンス
	uint32_t outputVerticesUavIndex; // 計算後の頂点データ
};

//モデルデータ
struct ModelData
{
	std::string name;										// 名前
	std::map<std::string, JointWeightData> skinClusterData;	// スキンクラスターデータ
	bool isNormalmap;
	// ノーマルマップを使っているか？
	Node rootNode;											// ノードデータ
	bool isAssimp;											// アシンプか
	bool isAmimetion;										// アニメーションするか
	bool isTangent;											// タンジェント
	SkinningSRVUAV skinning;								// スキニング用のSRV、UAVのインデックス
	std::vector <std::unique_ptr<Mesh>> mesh;				// メッシュデータ
	std::vector<uint32_t> cachedLineIndices_;				// ライン
	std::vector <std::unique_ptr<Material>> material;		// マテリアルデータ
	Animation animation;									// アニメーション
	float animationTime = 0.0f;								// アニメーション時間
	Skeleton skeleton;										// スケルトン
	SkinCluster skinCluster;								// スキンクラスター
};

class DirectXCommon;
namespace LoadModel {

	// メッシュ読み込み
	void LoadMesh(const aiScene* scene, ModelData& modelData, DirectXCommon* dxCommon);

	// ボーン読み込み
	void LoadBone(const aiScene* scene, ModelData& modelData, DirectXCommon* dxCommon);

	// マテリアル読み込み
	void LoadMaterial(const aiScene* scene, ModelData& modelData, DirectXCommon* dxCommon, const std::string& directoryPath);

	// アニメーション読み込み
	void LoadAnimation(ModelData& modelData, const std::string& directoryPath, const std::string& filename);

	// ノード読み込み
	Node ReadNode(aiNode* node);

}

class ModelCommon;
namespace CreateModel {

	// メッシュ状にライン生成
	void CreateMeshLine(ModelData& modelData,const std::vector<uint32_t>& indices);

	// スケルトン生成
	void CreateSkeleton(ModelData& modelData);

	// スキンクラスター生成
	void CreateSkinCluster(ModelData& modelData, ModelCommon* modelCommon);

	//
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
}




