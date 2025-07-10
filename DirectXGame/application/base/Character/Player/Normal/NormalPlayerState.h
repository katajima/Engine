#pragma once
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayerState.h"


class PlayerStateMove : public BasePlayerState
{
public:
	// コンストラクタ
	PlayerStateMove(BasePlayer* player);
	// 更新
	void Update();

	// 終了
	void Exit();

	// 初期化
	void Enter();
};


class PlayerStateAttack :public BasePlayerState
{
public:
	PlayerStateAttack(BasePlayer* player);

	void Update();

	// 終了
	void Exit();
	// 初期化
	void Enter();
};

class PlayerStateSpecial :public BasePlayerState
{
public:
	PlayerStateSpecial(BasePlayer* player);
	// 更新
	void Update();

	// 終了
	void Exit();
	// 初期化
	void Enter();
};