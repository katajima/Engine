#pragma once
#include <DirectXGame/engine/input/Input.h>
#include "GameEventController.h"
#include "GameData.h"

// 前方宣言
class BaseCharacterManager;
class GlobalVariables;
class SceneManager;
class Entity3DManager;

/// <summary>
/// ゲームの流れやルールを制御するクラス
/// </summary>
class GameFlowController {
public:

	// 初期化
	void Initialize(SceneManager* sceneManager,GlobalVariables* globalVariables, BaseCharacterManager* characterManager);

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
	Entity3DManager* entity3DManager = nullptr;
	BaseCharacterManager* characterManager = nullptr;
	GlobalVariables* globalVariables = nullptr;
	SceneManager* sceneManager = nullptr;
	Input* input = nullptr;
};