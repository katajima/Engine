#pragma once
#include <windows.h>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;
#include<vector>
#include<map>
#include <memory>


#include "DirectXGame/engine/struct/Structs3D.h"



class Model;
class Command;
class DXGIDevice;
class SrvManager;
class ModelCommon;
class DirectXCommon;

// テクスチャマネージャー
class ModelManager {
public:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void LoadModel(const std::string& filePath, const std::string& dire = "",const Vector2 texScale = {1,1});
	
	void LoadModelAmime(const std::string& filePath, const std::string& dire = "");

	Model* FindModel(const std::string& filePath);

	// 

	const std::map<std::string, std::unique_ptr<Model>>& GetModel() const { return models; }

	ModelCommon* GetModelCommon() { return modelCommon_.get(); }

private:
	std::map<std::string, std::unique_ptr<Model>> models;

	Command* command_;
	DXGIDevice* DXGIDevice_;
	SrvManager* srvManager_;
	DirectXCommon* dxCommon_;

	std::unique_ptr<ModelCommon> modelCommon_;

};
