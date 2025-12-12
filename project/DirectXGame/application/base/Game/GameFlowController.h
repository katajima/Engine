#pragma once
#include <DirectXGame/engine/input/Input.h>
#include "GameEventController.h"
#include "GameData.h"

// 前方宣言
class BaseCharacterManager;
namespace Engine {
	class GlobalVariables;
	class SceneManager;
	class Entity3DManager;
}
/// <summary>
/// ゲームの流れやルールを制御するクラス
/// </summary>
class GameFlowController {
public:

	// 初期化
	void Initialize(Engine::SceneManager* sceneManager, Engine::GlobalVariables* globalVariables, BaseCharacterManager* characterManager);

	// 更新
	void Update(float dt);

	// 描画
	void Draw();

private:

	// シーン変更
	void SceneChange();

private: // 
	// ゲームイベント制御クラス
	std::unique_ptr<GameEventController> gameEventController_ = nullptr;



	// ゲームプレイデータ
	GamePlayData gamePlayData_;

private: // 貰いもの
	Engine::Entity3DManager* entity3DManager = nullptr;
	BaseCharacterManager* characterManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::SceneManager* sceneManager = nullptr;
	Engine::Input* input = nullptr;
};