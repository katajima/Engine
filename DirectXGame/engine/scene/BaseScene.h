#pragma once



class SceneManager;
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

	virtual DirectXCommon* GetDxCommon() {return directXCommon_;}
private:
	// シーンマネージャ(借りてくる)
	SceneManager* sceneManager_ = nullptr;

	DirectXCommon* directXCommon_ = nullptr;
	//std::string sceneName = "";
};

