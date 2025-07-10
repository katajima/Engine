#pragma once

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/Animation/Animation.h"
\
#include "DirectXGame/engine/Mesh/ModelMesh.h"

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



//モデルデータ
struct ModelData
{
	std::string name;										// 名前
	bool isNormalmap;										// ノーマルマップを使っているか？
	std::unordered_map<uint32_t, Vector3> meshOffsetMap;	// オフセット位置
	Node rootNode;											// ノードデータ
	bool isAssimp;											// アシンプか
	bool isAmimetion;										// アニメーションするか
	bool isTangent;											// タンジェント
	std::vector<std::unique_ptr<ModelMesh>> mesh;			// メッシュデータ
	std::unique_ptr<ModelMesh> allMesh;
	std::vector<uint32_t> cachedLineIndices_;				// ライン
	std::map<std::string, Animation> animations;			// 名前付きアニメーション
	std::string currentAnimName = "Idle";					// 現在のアニメーション名前

	std::string previousAnimName = "";
	float blendTime = 0.0f;           // ブレンドに使う経過時間
	float blendDuration = 0.3f;      // ブレンドにかける合計時間
	bool isBlending = false;
	float animationTime = 0.0f;								// アニメーション時間
	Skeleton skeleton;										// スケルトン
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
	Node ReadNode(aiNode* node, std::unordered_map<uint32_t, Vector3>& meshOffsetMap);

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

namespace DebugModel {

	// スキニングデータImguiで表示
	void ImguiSkin(ModelData& modelData);

	void ImguiModel(ModelData& modelData);

}



