#pragma once
//#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayerState.h"
#include "DirectXGame/application/base/BaseClass/State/BaseState.h"


class PlayerStateMove : public MoveState
{
public:
	// コンストラクタ
	PlayerStateMove(BaseCharacter* player)
		: MoveState(player) {}

	// 更新
	void Update() override;

	// 終了
	void Exit()override;

	// 初期化
	void Enter() override;
};


class PlayerStateAttack :public AttackState
{
public:
	PlayerStateAttack(BaseCharacter* player)
		: AttackState(player) {}

	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;
};

class PlayerStateSpecial :public SpecialState
{
public:
	PlayerStateSpecial(BaseCharacter* player)
		: SpecialState(player) {}

	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;
};