#pragma once
#include<string>
#include <memory>
#include <functional>
class BasePlayer;
class BasePlayerState {
public:
	BasePlayerState(const std::string& name, BasePlayer* player) :name_(name), player_(player){};
	// 更新
	virtual void Update() = 0;
	// 終了
	virtual void Exit() = 0;
	// 初期化
	virtual void Enter() = 0;

	std::string GetName() { return name_; }
protected:
	// 状態名
	std::string name_;

	// 操作状態
	BasePlayer* player_ = nullptr;
};


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



