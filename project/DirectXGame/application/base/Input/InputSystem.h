#pragma once
#include "DirectXGame/engine/input/InputData.h"
#include "DirectXGame/application/base/Input/InputData.h"
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
	/// コントローラ入力を残したまま、キーボード・マウス由来のプレイヤー入力だけを抑制する。
	void ClearKeyboardMousePlayerInput();
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
	/// <summary>カメラなどが共通入力データを参照するための非所有参照を返す。</summary>
	const PlayerInputData* GetPlayerInputDataAddress() const { return &playerInputData_; }
	/// <summary>
	/// ゲーム操作の入力データ取得
	/// </summary>
	GameInputData GetGameInputData() const { return gameInputData_; }
	/// <summary>メニュー制御が共通入力データを参照するための非所有参照を返す。</summary>
	const Engine::MenuInputData* GetGameInputDataAddress() const { return &gameInputData_; }

	bool GetButtom(InputButton press, GamePadButton button) const;
	/// <summary>
	/// 指定キーがこのフレームで押されたかを取得する。
	/// </summary>
	/// <param name="key">DirectInputのキーコード。</param>
	bool IsTriggerKey(BYTE key) const;
	/// <summary>
	/// UI操作に使用するマウス位置を取得する。
	/// </summary>
	Vector2 GetMousePosition() const;
	/// <summary>
	/// UI操作用のマウスボタン押下開始を取得する。
	/// </summary>
	bool IsMouseTriggered(uint8_t button) const;
	/// <summary>
	/// UI操作用のマウスボタン押下中を取得する。
	/// </summary>
	bool IsMousePressed(uint8_t button) const;
	/// <summary>
	/// UI操作用のマウスボタン解放を取得する。
	/// </summary>
	bool IsMouseReleased(uint8_t button) const;

private:
	// プレイヤー操作の入力データ
	PlayerInputData playerInputData_;
	// 
	GameInputData gameInputData_;
private:
	// 入力クラス
	Engine::Input* input = nullptr;
};
