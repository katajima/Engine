#pragma once
#include"LevelData.h"



#include<json.hpp>


#include<memory>

#include "DirectXGame/engine/3d/Object/Object3d.h"

class Entity3DManager;
class LoadLevelData
{
public:

	void Initialize(Entity3DManager* entity3DManager, ModelManager* modelManager,Camera* camera, const std::string extensionName, const std::string fileName = "levelData/");

	void ReLoad();

	// 毎フレーム更新
	void Update();

	// 描画
	void Draw3D();
private:

	void CreateObject3d(LevelData* levelData);

private:
	// ロードデータ用
	std::vector<std::unique_ptr<Object3d>> objects;

	Entity3DManager* entity3DManager_;
	ModelManager* modelManager_;
private:
	std::string kFileName;
	std::string kExtension;
	std::string kDefaultBaseDirectory = "resources/";
	std::string extensionName;
};

