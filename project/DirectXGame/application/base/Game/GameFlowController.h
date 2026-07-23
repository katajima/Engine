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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::SceneManager* sceneManager, InputSystem* input, CameraManager* cameraManager, Engine::GlobalVariables* globalVariables, Character::CharacterManager* characterManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D();

	/// <summary>
	/// ゲームデータ
	/// </summary>
	Game::GameData GetGamePlayData() const { return gamePlayData_; }

	bool IsMove() const { return gameEventController_->IsMove(); }
private:

	/// <summary>
	/// シーン変更
	/// </summary>
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