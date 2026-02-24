#pragma once
#include "DirectXGame/engine/Math/MathFunctions.h"


// ロックオンタイプ
enum class LockOnType {
	kHit,	// 当てた相手
	kNear,	// 近い
};


// ロックオンデータ
struct LockOnData {
	// ロックオン開始時間
	float startTime = 0.0f;
	// ロックオン終了時間
	float endTime = 1.0f;
	// 半径
	float radius = 7.0f;
	//
	LockOnType type = LockOnType::kNear;
};
