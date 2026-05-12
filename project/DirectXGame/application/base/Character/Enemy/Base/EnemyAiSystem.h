#pragma once
#include "EnemyAttackSystem.h"

#include <memory>

class EnemyAiSystem {
public:

	// 初期化
	void Initialize();

	// 更新
	void Update(float dt);

private:
	// 攻撃システム
	std::unique_ptr<EnemyAttackSystem> attackSystem_ = nullptr;
};