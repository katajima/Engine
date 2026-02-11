#pragma once


/// <summary>
/// ジャンプに関連するデータを格納する構造体。
/// </summary>
struct JumpData {
	// ジャンプ力
	float power_ = 800.0f;
	// 上昇時の重力係数
	float upGravity_ = 15.0f;
	// 落下時の重力係数
	float fallGravity_ = 30.0f;
	// 入力受付時間
	float inputDelay_ = 0.1f;
	// 最大ジャンプ回数
	int maxJumpCount_ = 2;
};