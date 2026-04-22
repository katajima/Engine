#pragma once
#include "DirectXGame/engine/Math/MathFunctions.h"


// ロックオンタイプ
enum class LockOnType {
	kHit,	// 当てた相手
	kNear,	// 近い
};


// ロックオンデータ
struct LockOnData {
	// 半径
	float radius = 7.0f;
	//
	LockOnType type = LockOnType::kNear;
};
