#pragma once
#include <DirectXGame/engine/struct/Vector2.h>

struct PlayerInputData {
	// 移動スティック(左スティック)
	Vector2 moveShick{};
	// 視点スティック(右スティック)
	Vector2 lookStick{};

	// 押した瞬間
	// ジャンプ
	bool jumpPressed = false;
	bool jumpTrigger = false;
	// 回避
	bool dodgeTrigger = false;
	// スキル
	bool skillTrigger = false;
	// 必殺技
	bool specialTrigger = false;

	// 押している
	// ダッシュ
	bool dashHeld = false;
	// ロックオン
	bool lockOnHeld = false;


	// コントローラが接続されているか
	bool isControllerConnected = false;
};

/// <summary>
/// ゲーム操作に使う入力データの構造体
/// </summary>
struct GameInputData {
	// 移動スティック(左スティック)
	Vector2 moveShick{};
	// 決定
	bool decisionTrigger = false;
	// ポーズ
	bool pauseTrigger = false;
};