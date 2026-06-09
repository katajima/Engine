#pragma once
#include "EnemyAttackRequest.h"

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// Combat DirectorとAttack Tokenが共有する攻撃候補。
	/// 敵本体と、その敵が持っている攻撃要求を1セットで扱う。
	/// </summary>
	struct EnemyAttackCandidate {
		BaseEnemy* enemy = nullptr;				// 攻撃候補になっている敵
		EnemyAttackRequest* request = nullptr;	// 敵が保持している攻撃要求
	};
}
