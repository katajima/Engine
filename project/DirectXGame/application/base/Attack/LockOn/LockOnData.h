#pragma once
#include "DirectXGame/engine/Math/MathFunctions.h"


// ロックオンタイプ
enum class LockOnType {
	kHit,	// 当てた相手
	kNear,	// 近い
};

// ターゲットまでの移動タイプ
enum class TargetMoveType {
	kNone,		// なし
	kMove,		// 移動
	kTeleport,	// 瞬間移動
	kInterpolation	// 補間移動
};

// ロックオンデータ
struct LockOnData {
	// 半径察知
	float radius = 7.0f;
	// ロックオン	
	LockOnType type = LockOnType::kNear;
	// ターゲットの距離でどこまで近づくか
	float moveTargetRadius = 1.0f;
	// ターゲットまで移動するときの移動タイプ
	TargetMoveType targetMoveType = TargetMoveType::kMove;

};
