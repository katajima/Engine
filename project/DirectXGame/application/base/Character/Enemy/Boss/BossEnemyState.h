#pragma once
#include "DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemyState.h"


// 移動
class BossEnemyStateMove : public BaseEnemyState
{
public:
	// コンストラクタ
	BossEnemyStateMove(BaseEnemy* enemy);
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
class BossEnemyStateAttack :public BaseEnemyState
{
public:
	BossEnemyStateAttack(BaseEnemy* enemy);

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
class BossEnemyStateSpecial :public BaseEnemyState
{
public:
	BossEnemyStateSpecial(BaseEnemy* enemy);
	// 更新
	void Update();

	// 終了
	void Exit();
	// 初期化
	void Enter();
};

// 死亡
class BossEnemyStateDie :public BaseEnemyState
{
public:
	BossEnemyStateDie(BaseEnemy* enemy);
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