#pragma once



class SceneManager;
class Entity3DManager;
class DirectXCommon;
class BaseScene
{
public:
	virtual ~BaseScene() = default;

	// 初期化
	virtual void Initialize() = 0;

	// 終了
	virtual void Finalize() = 0;

	// 毎フレーム更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw3D() = 0;
	
	virtual void Draw2D() = 0;

	virtual void SetSceneManader(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

	virtual void SetDirectXCommon(DirectXCommon* dxCommon) { directXCommon_ = dxCommon; }

	void SetEntity3DManager(Entity3DManager* entity3DManager) { entity3DManager_ = entity3DManager; }

	virtual DirectXCommon* GetDxCommon() {return directXCommon_;}

	Entity3DManager* GetEntity3DManager() { return entity3DManager_; }
private:
	// シーンマネージャ(借りてくる)
	SceneManager* sceneManager_ = nullptr;

	DirectXCommon* directXCommon_ = nullptr;
	Entity3DManager* entity3DManager_;
};

