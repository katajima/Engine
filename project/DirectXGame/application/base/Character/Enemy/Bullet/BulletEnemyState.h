#pragma once
#include "DirectXGame/application/base/State/BaseMainState.h"

class BulletEnemyStateMove : public MoveState {
public:
	// コンストラクタ
	BulletEnemyStateMove(BaseCharacter* enemy)
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
class BulletEnemyStateAttack :public AttackState
{
public:
	BulletEnemyStateAttack(BaseCharacter* enemy)
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
class BulletEnemyStateDie :public DieState
{
public:
	BulletEnemyStateDie(BaseCharacter* enemy)
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

