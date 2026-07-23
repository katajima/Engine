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
	/// <summary>
	/// プレイヤー操作だけを空にする
	/// </summary>
	void ClearPlayerInput();
private:
	/// <summary>
	/// プレイヤー操作の入力データの更新
	/// </summary>
	void PlayerInputUpdate(float dt);
	/// <summary>
	/// ゲーム操作の入力データの更新
	/// </summary>
	void GameInputUpdate(float dt);
public:
	/// <summary>
	/// 入力データ取得
	/// </summary>
	PlayerInputData GetPlayerInputData() const { return playerInputData_; }
	/// <summary>
	/// ゲーム操作の入力データ取得
	/// </summary>
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
