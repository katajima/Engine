#pragma once
#include "DirectXGame/engine/struct/Vector2.h"

/// <summary>
/// アプリケーション固有のゲーム画面操作入力を保持する構造体。
/// </summary>
struct GameInputData {
	// メニュー移動に使う左スティック入力。
	Vector2 moveShick{};
	// 決定入力が発生したか。
	bool decisionTrigger = false;
	// ポーズ入力が発生したか。
	bool pauseTrigger = false;
};