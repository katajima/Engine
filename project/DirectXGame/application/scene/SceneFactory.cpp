#include "SceneFactory.h"
#include <DirectXGame/engine/Base/pch.h>


BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	if (sceneName == "GAMEPLAY") {
		newScene = new GamePlayScene();
	} 
	if (sceneName == "TEST") {
		newScene = new TestScene();
	}
	if (sceneName == "SELECT") {
		newScene = new SelectScene();
	}	
	if (sceneName == "SAMPLE") {
		newScene = new SampleScene();
	}



	return newScene;
}
