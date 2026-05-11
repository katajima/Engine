#pragma once
#include <string>
#include <vector>


enum class DeathType {
	Normal,			// その場で倒れる
	KnockBack,		// 後方吹き飛び
	Launch,			// 打ち上げ死亡
	Air,			// 空中落下死亡
	Explode,		// 爆発
};

// 死亡データ構造体
struct DeathData {
	// 死亡までの時間
	float dieTimer = 0.0f;
	// 死亡エフェクトの名前
	std::vector<std::string> deathEffectNames;	
	
};