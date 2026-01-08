#pragma once
#include "DirectXGame/application/base/State/BaseMainState.h"
#include "NormalEnemySubState.h"

// 移動
class EnemyStateMove : public MoveState
{
public:
	// コンストラクタ
	EnemyStateMove(BaseCharacter* enemy)
		: MoveState(enemy){}


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
class EnemyStateAttack :public AttackState
{
public:
	EnemyStateAttack(BaseCharacter* enemy)
		: AttackState(enemy) {
	}
	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;
private:
	std::unique_ptr<SubStateMachine<AttackSubState, BaseAttackSubState>> subStateMachine_;
};

/// <summary>
/// 死亡状態
/// </summary>
class EnemyStateDie :public DieState
{
public:
	EnemyStateDie(BaseCharacter* enemy)
		: DieState(enemy) {}

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

