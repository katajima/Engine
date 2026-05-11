#pragma once
#include"LevelData.h"
#include<json.hpp>
#include<memory>



class CameraManager;
class BaseCamera;
namespace Engine {
	class EntityManager;
	class Object3d;
	class ModelManager;
	class Camera;
	class Lights;
}

/// <summary>
/// レベルデータを管理するクラス。
/// </summary>
class LoadLevelData
{
public:

	// 初期化
	void Initialize(Engine::EntityManager* entity3DManager, Engine::ModelManager* modelManager, Engine::Camera* camera, 
		const std::string extensionName, const std::string fileName = "levelData/");
	// リロード
	void ReLoad();

	// 毎フレーム更新
	void Update();

	// 描画
	void Draw3D();

	// カメラマネージャー設定
	void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };

public:
	// レベルデータ取得
	LevelData* GetLevelData() { return levelData_.get(); };
	// オブジェクト取得
	std::vector<Engine::Object3d*>GetObjects() { return objects_; }
	// ライト取得
	std::vector< std::shared_ptr<Engine::Lights>>GetLights() { return lights_; }

	// データクリア
	void ClearData();
private:
	// オブジェクト3D生成
	void CreateObject3d(LevelData* levelData);
	// カメラ生成
	void CreateCamera(LevelData* levelData);
	// ライト生成
	void CreateLight(LevelData* levelData);
private:
	Engine::EntityManager* entityManager = nullptr;
	Engine::ModelManager* modelManager = nullptr;
	CameraManager* cameraManager = nullptr;
	std::vector<Engine::Object3d*> objects_;					// オブジェクト
	std::vector<std::unique_ptr<BaseCamera>> cameras_;			// カメラ
	std::vector<std::shared_ptr<Engine::Lights>> lights_;		// ライト

private:
	std::string kFileName;
	std::string kExtension;
	std::string kDefaultBaseDirectory = "resources/";
	std::string extensionName;
	// レベルデータ格納用インスタンスを生成 
	std::unique_ptr<LevelData> levelData_ = nullptr;
};

