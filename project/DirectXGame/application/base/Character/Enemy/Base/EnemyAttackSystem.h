#pragma once
#include <vector>
#include "EnemyAttackRequest.h"

struct EnemyAttackData {
	
};


// 敵の攻撃システム(攻撃順番など)
class EnemyAttackSystem {
public:

	// 初期化
	void Initialize();
	// 更新
	void Update(float dt);
	// 攻撃クールダウンの設定
	void SetAttackCooldown(float cooldown) { attackCooldown = cooldown; }

private:
	float attackCooldown;	// 攻撃クールダウン
};