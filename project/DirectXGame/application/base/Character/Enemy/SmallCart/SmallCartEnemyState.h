#pragma once
#include "DirectXGame/application/base/State/BaseMainState.h"




class BaseCharacter;

// 移動
class SmallCartEnemyMoveState : public MoveState
{
public:
	// コンストラクタ
	SmallCartEnemyMoveState(BaseCharacter* enemy)
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
/// 死亡状態
/// </summary>
class SmallCartEnemyDieState :public DieState
{
public:
	SmallCartEnemyDieState(BaseCharacter* enemy)
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

