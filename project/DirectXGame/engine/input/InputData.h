#pragma once
#include <DirectXGame/engine/struct/Vector2.h>

struct PlayerInputData {
	// 移動スティック(左スティック)
	Vector2 moveShick{};
	// 視点操作に使う右スティック入力。カメラやゲーム側で共通利用する。
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
/// エンジン共通のメニュー操作入力を保持する構造体。
/// </summary>
namespace Engine {
struct MenuInputData {
	// メニュー移動に使う左スティック入力。
	Vector2 moveShick{};
	// 決定入力が発生したか。
	bool decisionTrigger = false;
	// ポーズ入力が発生したか。
	bool pauseTrigger = false;
};
}
