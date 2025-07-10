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

public:
	LevelData* GetLevelData() { return levelData_; };

	std::vector<Object3d*>GetObjects() { return objects_; }

private:
	// オブジェクト3D生成
	void CreateObject3d(LevelData* levelData);
	// スポーンポイントの生成
	//void CreateSpawnPoint(LevelData* levelData);


private:
	Entity3DManager* entity3DManager_;
	ModelManager* modelManager_;
	std::vector<Object3d*> objects_;
private:
	std::string kFileName;
	std::string kExtension;
	std::string kDefaultBaseDirectory = "resources/";
	std::string extensionName;
	// レベルデータ格納用インスタンスを生成 
	LevelData* levelData_ = nullptr;
};

