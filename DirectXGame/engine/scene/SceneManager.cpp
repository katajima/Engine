#include"SceneManager.h"
#include"cassert"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void SceneManager::Update()
{
	// 次のシーンの予約があるなら
	if (nextScene_) {
		// 旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}
		// シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;

		// シーンマネージャをセット
		scene_->SetSceneManader(this);

		//次のシーンを初期化する
		scene_->SetInput(input_);
		scene_->SetGlobalVariables(globalVariables_);
		scene_->SetDirectXCommon(directXCommon_);
		scene_->SetEntity3DManager(entity3DManager_);
		scene_->SetEntity2DManager(entity2DManager_);
		scene_->Initialize();
		SetCamera(scene_->GetCamara()); // カメラ情報をもらう
		scene_->GetInput()->Update();
	}
	
	//実行シーンを更新する
	scene_->Update();
}

void SceneManager::Draw3D()
{

	scene_->Draw3D();
}

void SceneManager::Draw2D()
{
	scene_->Draw2D();

}

SceneManager::~SceneManager()
{
	// 最後のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//次のシーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
	this->sceneName = sceneName;

}
