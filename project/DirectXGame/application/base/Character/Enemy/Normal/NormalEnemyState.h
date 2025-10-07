#pragma once
//#include "DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemyState.h"
#include "DirectXGame/application/base/BaseClass/State/BaseState.h"
// 移動
class EnemyStateMove : public MoveState
{
public:
	// コンストラクタ
	EnemyStateMove(BaseCharacter* enemy)
		: MoveState(enemy){}


	// 更新
	void Update();

	// 終了
	void Exit();

	// 初期化
	void Enter();
private:
	float rootTimer_ = 3.0f;
	float timer_ = 0.0f;
};

// 攻撃
class EnemyStateAttack :public AttackState
{
public:
	EnemyStateAttack(BaseCharacter* enemy)
		: AttackState(enemy) {}

	void Update();

	// 終了
	void Exit();
	// 初期化
	void Enter();
private:
	float timer_ = 0.0f;
	float attackTimer_ = 1.0f;
	float attackSpeed_ = 10.0f; // 攻撃速度
	Vector3 lockonPos_;
	Vector3 subPos_;
};

// 必殺技
class EnemyStateSpecial :public SpecialState
{
public:
	EnemyStateSpecial(BaseCharacter* enemy)
		: SpecialState(enemy) {}
	
	// 更新
	void Update();

	// 終了
	void Exit();
	// 初期化
	void Enter();
};

// 死亡
class EnemyStateDie :public DieState
{
public:
	EnemyStateDie(BaseCharacter* enemy)
		: DieState(enemy) {}

	// 更新
	void Update();

	// 終了
	void Exit();
	// 初期化
	void Enter();

private:
	float dieTimer_ = 2.0f;
	float timer_ = 0.0f;
};