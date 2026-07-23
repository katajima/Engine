#pragma once
#include "DirectXGame/engine/scene/AbstractSceneFactory.h"


class TitleScene;
class GamePlayScene;
class TestScene;
class SelectScene;
class SampleScene;
class ResultScene;
class TutorialScene;
class CharacterDebugScene;
class CustomScene;
class EffectScene;
class BaseScene;

// このゲーム用のシーン工場
/// <summary>
/// SceneFactoryを管理・実装するクラス。
/// </summary>
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

