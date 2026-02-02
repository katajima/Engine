#pragma once
#include "DirectXGame/application/base/State/BaseMainState.h"



class BaseCharacter;

// 移動
class SmallRangeEnemyMoveState : public MoveState
{
public:
	// コンストラクタ
	SmallRangeEnemyMoveState(BaseCharacter* enemy)
		: MoveState(enemy) {
	}


	// 更新
	void Update() override;

	// 終了
	void Exit() override;

	// 初期化
	void Enter() override;
private:
	float rootTimer_ = 5.0f;
	float timer_ = 0.0f;
};

/// <summary>
/// 攻撃
/// </summary>
class SmallRangeEnemyAttackState :public AttackState
{
public:
	SmallRangeEnemyAttackState(BaseCharacter* enemy)
		: AttackState(enemy) {
	}
	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;
private:
	float timer_ = 0.0f;
};

/// <summary>
/// 死亡状態
/// </summary>
class SmallRangeEnemyDieState :public DieState
{
public:
	SmallRangeEnemyDieState(BaseCharacter* enemy)
		: DieState(enemy) {
	}

	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;

private:
	float dieTimer_ = 2.0f;
	float timer_ = 0.0f;
};



