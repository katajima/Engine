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
	/// <summary>
	/// デストラクタ
	/// </summary>
	~LoadLevelData();

	// 初期化
	void Initialize(Engine::EntityManager* entity3DManager, Engine::ModelManager* modelManager, Engine::Camera* camera, 
		const std::string extensionName, const std::string fileName = "levelData/");
	/// <summary>
	/// リロード
	/// </summary>
	void ReLoad();

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw3D();

	/// <summary>
	/// カメラマネージャー設定
	/// </summary>
	void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };

public:
	/// <summary>
	/// レベルデータ取得
	/// </summary>
	LevelData* GetLevelData() { return levelData_.get(); };
	/// <summary>
	/// オブジェクト取得
	/// </summary>
	std::vector<Engine::Object3d*>GetObjects() { return objects_; }
	/// <summary>
	/// ライト取得
	/// </summary>
	std::vector< std::shared_ptr<Engine::Lights>>GetLights() { return lights_; }

	/// <summary>
	/// データクリア
	/// </summary>
	void ClearData();
private:
	/// <summary>
	/// オブジェクト3D生成
	/// </summary>
	void CreateObject3d(LevelData* levelData);
	/// <summary>
	/// カメラ生成
	/// </summary>
	void CreateCamera(LevelData* levelData);
	/// <summary>
	/// ライト生成
	/// </summary>
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

