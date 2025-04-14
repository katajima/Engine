#pragma once
#include"BaseScene.h"
#include"AbstractSceneFactory.h"


class DirectXCommon;
class Entity3DManager;
class SceneManager
{
public:
	static SceneManager* instance;

	static SceneManager* GetInstance();
	
	void Finalize();

	void Update();

	void Draw2D();
	
	void Draw3D();
	
	

	~SceneManager();

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

	void SetDirectXCommon(DirectXCommon* directXCommon) { directXCommon_ = directXCommon; }

	void SetEntity3DManager(Entity3DManager* entity3DManager) { entity3DManager_ = entity3DManager; }

	DirectXCommon* GetDirectXCommon() { return directXCommon_; };

	Entity3DManager* GetEntity3DManager() { return entity3DManager_; }

	/// <summary>
	/// 次のシーン予約
	/// </summary>
	/// <param name="sceneName"></param>
	void ChangeScene(const std::string& sceneName);

	bool IsNowScene(std::string name) const {
		if (sceneName == name) {
			return true;
		}
		return false; 
	};

private:
	// 今のシーン(実行中シーン)
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
	std::string sceneName = "";
	// シーンファクトリー (借りてくる)
	AbstractSceneFactory* sceneFactory_ = nullptr;
	DirectXCommon* directXCommon_;
	Entity3DManager* entity3DManager_;
};

