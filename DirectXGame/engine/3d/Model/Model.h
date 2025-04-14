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


class Material;
class Model
{
public:
	//モデルデータ
	struct ModelData
	{
		std::map<std::string, JointWeightData> skinClusterData;
		//MaterialData material;
		bool isNormalmap;
		Node rootNode;
		bool isAssimp;
		uint32_t skinningSrvindex;
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



	void Initialize(ModelCommon* modelCommon,const std::string& directorypath,const std::string& filename,const std::string& file = "", const Vector2 texScale = {1,1});

	void InitializeAnime(ModelCommon* modelCommon,const std::string& directorypath,const std::string& filename,const std::string& file = "");

	
	
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


