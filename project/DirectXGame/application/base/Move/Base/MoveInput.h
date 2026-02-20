#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"

struct PlayerInputData {
	// 移動スティック(左スティック)
	Vector3 moveShick;
	// 視点スティック(右スティック)
	Vector3 lookStick;

	// 押した瞬間
	// ジャンプ
	bool jumpPressed;
	// 回避
	bool dodgePressed;
	// スキル
	bool skillPressed;
	// 必殺技
	bool specialPrassed;
	
	// 押している
	// ダッシュ
	bool dashHeld;
	// ロックオン
	bool lockOnHeld;

};



