#include "SceneFactory.h"
#include <DirectXGame/engine/Base/pch.h>


BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {				// タイトル
		newScene = new TitleScene();
	}
	if (sceneName == "GAMEPLAY") {			// ゲームプレイ
		newScene = new GamePlayScene();
	} 
	if (sceneName == "TEST") {				// テスト
		newScene = new TestScene();
	}
	if (sceneName == "SELECT") {			// セレクト
		newScene = new SelectScene();
	}	
	if (sceneName == "SAMPLE") {			// サンプル
		newScene = new SampleScene();
	}



	return newScene;
}
