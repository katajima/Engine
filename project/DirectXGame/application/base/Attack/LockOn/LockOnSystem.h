#pragma once
#include "vector"
#include "LockOnData.h"

// 前方宣言
class BaseCharacter;
class BaseEnemy;


/// <summary>
/// ロックオンシステム
/// </summary>
class LockOnSystem {
public:



	// 初期化
	void Initialize(BaseCharacter* owner);

	// 更新
	void Update(float dt);

	// ターゲット指定
	void SetTargets(const std::vector<BaseCharacter*>& targetCharacters) { targetCharacters_ = targetCharacters; };


private:
	BaseCharacter* owner = nullptr;
	// 攻撃対象キャラクターリスト
	std::vector<BaseCharacter*> targetCharacters_;
private:


};
