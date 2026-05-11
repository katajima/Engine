#pragma once
#include "DirectXGame/engine/struct/Vector3.h"

/// <summary>
/// 移動関係データ構造体
/// </summary>
struct MovementData {
	// 移動速度
	float moveSpeed;
	// 移動方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };
};