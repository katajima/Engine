#pragma once
#include "DirectXGame/engine/scene/AbstractSceneFactory.h"
#include"TitleScene.h"
#include"GamePlayScene.h"
#include"TestScene.h"
#include "SelectScene.h"
#include "SampleScene.h"
#include "ResultScene.h"

// このゲーム用のシーン工場
class SceneFactory : public Engine::AbstractSceneFactory
{
public:

	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName"></param>
	/// <returns></returns>
	std::unique_ptr<Engine::BaseScene> CreateScene(const std::string& sceneName) override;

};

