#pragma once

// 移動タイプ
enum class MoveType
{
	LINEAR,		// 線形移動
	ACCELERATE, // 加速移動
	DECELERATE, // 減速移動
};


// 移動データ
struct MoveData
{
	// 加速度
	float speedAcceleration = 0.1f;
	// 最大速度
	float maxSpeed = 1.0f;
	// 最小速度
	float minSpeed = 0.0f;
	// 移動タイプ
	MoveType moveType = MoveType::LINEAR;
	// 回転補間速度
	float rotationSpeed = 0.1f;
	// スティックの倒し方で速度を変化するかどうか
	bool isStickToSpeed = true;
	// 空中での速度制限をするかどうか
	bool isLimitAirSpeed = true;
	// 空中での速度係数 
	float airSpeedRate = 0.85f;

	// 歩きから走り状態への境界指定
	float moveStateThreshold = 10.0f; // 移動状態のしきい値

	// 歩きアニメーション最小速度
	float animetionSpeedMinWalk = 0.1f;
	// 歩きアニメーション最大速度
	float animetionSpeedMaxWalk = 2.0f;

	// 走りアニメーション最小速度
	float animetionSpeedMinRun = 1.0f;
	// 走りアニメーション最大速度
	float animetionSpeedMaxRun = 3.0f;

	// 走りと歩きで同じアニメーションか
	bool isSameAnimation = true;
};