#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"

struct InputData {
	// 移動スティック(左スティック)
	Vector2 moveShick;
	// 視点スティック(右スティック)
	Vector2 lookStick;

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
