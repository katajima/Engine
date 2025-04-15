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
#include <iostream>
#include <memory>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



class ModelCommon;
class DirectXCommon;

class Material;
class Model
{
public:
	struct SkinningSRVUAV {
		uint32_t wellSrvIndex; //Well
		uint32_t inputVerticesIndex; // 入力頂点
		uint32_t influencesIndex; // 入力インフルエンス
		uint32_t outputVerticesUavIndex; // 計算後の頂点データ
	};


	//モデルデータ
	struct ModelData
	{
		std::map<std::string, JointWeightData> skinClusterData;
		//MaterialData material;
		bool isNormalmap;
		Node rootNode;
		bool isAssimp;
		SkinningSRVUAV skinning;
		std::vector <std::unique_ptr<Mesh>> mesh;
		std::vector<uint32_t> cachedLineIndices_;
		std::vector <std::unique_ptr<Material>> material;
		std::string name;
	};
	
	// アニメーション
	Animation animation;
	float animationTime = 0.0f;
	Skeleton skeleton;
	SkinCluster skinCluster;


public:



	void Initialize(DirectXCommon* dxCommon,ModelCommon* modelCommon,const std::string& directorypath,const std::string& filename,const std::string& file = "", const Vector2 texScale = {1,1});

	void InitializeAnime(DirectXCommon* dxCommon,ModelCommon* modelCommon,const std::string& directorypath,const std::string& filename,const std::string& file = "");

	
	
	void Draw();
	
	void DrawSkinning();

	ModelData& GetModelData(){ return modelData; }

	
	static Node ReadNode(aiNode* node);

	
private:

	ModelData LoadMesh(const aiScene* _scene);

	void GenerateMeshLine(const std::vector<uint32_t>& indices);
	const std::vector<uint32_t>& GetCachedLineIndices() const { return modelData.cachedLineIndices_; }

public:


	// Objファイルのデータ
	ModelData modelData;
private:
	ModelCommon* modelCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	DirectXCommon* dxCommon_;
	
	bool useNormalMap = false;
	bool useSpecularMap = false;
public:	
	ModelData LoadOdjFileAssimp(const std::string& directoryPath, const std::string& filename, const Vector2 texScale = {1,1});
	
	ModelData LoadOdjFileAssimpAmime(const std::string& directoryPath, const std::string& filename);

	static Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	
	
	//
	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);
public:

};


