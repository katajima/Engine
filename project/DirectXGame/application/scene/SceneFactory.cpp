#include "SceneFactory.h"
#include <DirectXGame/engine/Base/pch.h>
#include"TitleScene.h"
#include"GamePlayScene.h"
#include"TestScene.h"
#include "SelectScene.h"
#include "SampleScene.h"
#include "ResultScene.h"
#include "TutorialScene.h"
#include "CharacterDebugScene.h"
#include "CustomScene.h"

std::unique_ptr<Engine::BaseScene> SceneFactory::CreateScene(const std::string& sceneName)
{
    // シーン名に対応するシーンクラスを生成する
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
    if(sceneName == "TUTORIAL") {              // チュートリアル
        return std::make_unique<TutorialScene>();
	}
    if (sceneName == "CHARACTER") {             // キャラクターのデバッグシーン
        return std::make_unique<CharacterDebugScene>();
    }
    if (sceneName == "CUSTOM") {             // カスタムシーン
        return std::make_unique<CustomScene>();
    }

    // 未登録のシーン名が渡された場合は生成できないことを通知する
    return nullptr;
}
