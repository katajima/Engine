#include "SceneFactory.h"
#include <DirectXGame/engine/Base/pch.h>


std::unique_ptr<Engine::BaseScene> SceneFactory::CreateScene(const std::string& sceneName)
{
    if (sceneName == "TITLE") {                 // タイトル
        return std::make_unique<TitleScene>();
    }
    if (sceneName == "GAMEPLAY") {              // ゲームプレイ
        return std::make_unique<GamePlayScene>();
    }
    if (sceneName == "TEST") {                  // テスト
        return std::make_unique<TestScene>();
    }
    if (sceneName == "SELECT") {                // セレクト
        return std::make_unique<SelectScene>();
    }
    if (sceneName == "SAMPLE") {                // サンプル
        return std::make_unique<SampleScene>();
    }
	if (sceneName == "RESULT") {                // リザルト
		return std::make_unique<ResultScene>();
    }

    return nullptr;
}
