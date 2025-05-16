#pragma once

// engine
#include"DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/Animation/Animation.h"
#include"DirectXGame/engine/Mesh/Mesh.h"
#include"DirectXGame/engine/struct/Material.h"
#include"DirectXGame/engine/Material/Material.h"


#include"ModelData.h"

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

	// 初期化
	void Initialize(DirectXCommon* dxCommon,ModelCommon* modelCommon,const std::string& directorypath,const std::string& filename,const std::string& file = "");

	// 通常描画
	void Draw();
	
	// スキニング用描画
	void DrawSkinning();

	// ModelData取得
	ModelData& GetModelData(){ return modelData; }	
public:
	// モデルのデータ
	ModelData modelData;
private:
	ModelCommon* modelCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	DirectXCommon* dxCommon_;
	
public:	
	ModelData LoadOdjFileAssimpAmime(const std::string& directoryPath, const std::string& filename);
};