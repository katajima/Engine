#pragma once
#include"LevelData.h"



#include<json.hpp>


#include<memory>

#include "DirectXGame/engine/3d/Object/Object3d.h"

class CameraManager;
class BaseCamera;
class Entity3DManager;

/// <summary>
/// レベルデータを管理するクラス。
/// </summary>
class LoadLevelData
{
public:

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, ModelManager* modelManager,Camera* camera, const std::string extensionName, const std::string fileName = "levelData/");
	// リロード
	void ReLoad();

	// 毎フレーム更新
	void Update();

	// 描画
	void Draw3D();

	// カメラマネージャー設定
	void SetCameraManager(CameraManager* cameraManager) { cameraManager_ = cameraManager; };

public:
	// レベルデータ取得
	LevelData* GetLevelData() { return levelData_.get(); };
	// オブジェクト取得
	std::vector<Object3d*>GetObjects() { return objects_; }
	// ライト取得
	std::vector< std::shared_ptr<Lights>>GetLights() { return lights_; }


private:
	// オブジェクト3D生成
	void CreateObject3d(LevelData* levelData);
	// カメラ生成
	void CreateCamera(LevelData* levelData);
	// ライト生成
	void CreateLight(LevelData* levelData);

	// データクリア
	void ClearData();

private:
	Entity3DManager* entity3DManager_ = nullptr;
	ModelManager* modelManager_ = nullptr;
	CameraManager* cameraManager_ = nullptr;
	std::vector<Object3d*> objects_;					// オブジェクト
	std::vector<std::unique_ptr<BaseCamera>> cameras_;	// カメラ
	std::vector<std::shared_ptr<Lights>> lights_;		// ライト

private:
	std::string kFileName;
	std::string kExtension;
	std::string kDefaultBaseDirectory = "resources/";
	std::string extensionName;
	// レベルデータ格納用インスタンスを生成 
	std::unique_ptr<LevelData> levelData_ = nullptr;
};

