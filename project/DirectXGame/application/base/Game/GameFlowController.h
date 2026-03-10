#pragma once
#include <DirectXGame/engine/input/Input.h>
#include "GameEventController.h"
#include "DirectXGame/application/base/Game/Base/GameData.h"

// 前方宣言
namespace Character {
	class CharacterManager;
}
namespace Engine {
	class GlobalVariables;
	class SceneManager;
	class EntityManager;
}
class InputSystem;

/// <summary>
/// ゲームの流れやルールを制御するクラス
/// </summary>
class GameFlowController {
public:

	// 初期化
	void Initialize(Engine::SceneManager* sceneManager, InputSystem* input, CameraManager* cameraManager, Engine::GlobalVariables* globalVariables, Character::CharacterManager* characterManager);

	// 更新
	void Update(float dt);

	// 描画
	void Draw();

	// 描画2d
	void Draw2D();

	// ゲームデータ
	Game::GameData GetGamePlayData() const { return gamePlayData_; }

private:

	// シーン変更
	void SceneChange();

private: // 
	// ゲームイベント制御クラス
	std::unique_ptr<Game::GameEventController> gameEventController_ = nullptr;
	// ゲームプレイデータ
	Game::GameData gamePlayData_;
private:
	// ゲームが終わったか
	bool isEndgame_ = false;

private: // 貰いもの
	Engine::EntityManager* entityManager = nullptr;
	Character::CharacterManager* characterManager = nullptr;
	CameraManager* cameraManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::SceneManager* sceneManager = nullptr;
	Engine::Input* input = nullptr;
};