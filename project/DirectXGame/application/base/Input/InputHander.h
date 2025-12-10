#pragma once
#include<memory>

// 前方宣言
class BaseCharacter;


// コマンド
class ICommand 
{
public:
	virtual ~ICommand();
	virtual void Exec(BaseCharacter& character) = 0;

};

// 移動コマンド
class MoveCommand :public ICommand 
{
public:
	void Exec(BaseCharacter& character) override;
};
// ジャンプコマンド
class JampCommand : public ICommand
{
public:
	void Exec(BaseCharacter& character) override;
};
// 攻撃コマンド
class AttackCommand : public ICommand
{
public:
	void Exec(BaseCharacter& character) override;
};

// 攻撃コマンド
class HeavyAttackCommand : public ICommand
{
public:
	void Exec(BaseCharacter& character) override;
};


// 前方宣言
class Input;
// インプットハンドラー
class InputHander 
{
public:
	// インプット設定
	void SetInput(Input* input) { input_ = input; };
	// ハンドルインプット取得
	ICommand* HandleInput();
	// 割り当て移動
	void AssignMoveCommandPad();
	// 割り当てジャンプ
	void AssignJampCommandPad();
	// 割り当て攻撃
	void AssignAttackCommandPad();
	// 割り当て攻撃
	void AssignHeavyAttackCommandPad();

private:
	Input* input_;

	std::unique_ptr<ICommand> movePad;
	std::unique_ptr<ICommand> jampPad;
	std::unique_ptr<ICommand> attackPad;
	std::unique_ptr<ICommand> attackHeavyPad;

};