#pragma once


// ダッシュ用データ
struct  DashData {
	// ダッシュ初速度
	float startSpeed = 100.0f;
	// 加速
	float acceleration = 0.0f;
	// 減衰
	float friction = 10.0f;
	// 最大ダッシュ時間
	float maxTime = 0.15f;
	// ダッシュ中に重力適用させるかのフラグ
	bool isDashGravity = false;
};