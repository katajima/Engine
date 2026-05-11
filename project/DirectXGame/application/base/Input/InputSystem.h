#pragma once
#include "InputData.h"
#include "DirectXGame/engine/input/Input.h"

enum class InputButton {
	kPressed,
	kTriggered,
	kReleased,
};

/// <summary>
/// 入力の値の管理
/// </summary>
class InputSystem {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::Input* input);


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);
private:
	// プレイヤー操作の入力データの更新
	void PlayerInputUpdate(float dt);
	// ゲーム操作の入力データの更新
	void GameInputUpdate(float dt);
public:
	// 入力データ取得
	PlayerInputData GetPlayerInputData() const { return playerInputData_; }
	// ゲーム操作の入力データ取得
	GameInputData GetGameInputData() const { return gameInputData_; }

	bool GetButtom(InputButton press, GamePadButton button) const;

private:
	// プレイヤー操作の入力データ
	PlayerInputData playerInputData_;
	// 
	GameInputData gameInputData_;
private:
	// 入力クラス
	Engine::Input* input = nullptr;
};