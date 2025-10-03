#pragma once
#include<memory>

enum class AttackInput {
	Light,          // 弱攻撃
	Heavy,          // 強攻撃
	Special,        // 必殺技など特殊攻撃
	JumpAttack,     // 空中攻撃
	DashAttack,     // ダッシュ中に攻撃
	Charge,         // チャージ攻撃（ボタン長押し）
	Cancel,         // コンボキャンセル（ステップや回避などに移行）
	Parry,          // 敵の攻撃に対するカウンター
};


class BaseCharacter;
class AttackICommand
{
public:
	virtual ~AttackICommand();
	virtual void Exec(BaseCharacter& character) = 0;

};

// 弱攻撃
class AttackLight : public AttackICommand
{
	void Exec(BaseCharacter& character) override;
};
// 強攻撃
class AttackHeavy : public AttackICommand
{
	void Exec(BaseCharacter& character) override;
};


class Input;
class AttackInputHander 
{
public:
	void SetInput(Input* input) { input_ = input; };

	AttackICommand* HandleInput();

	void AssignAttack();

private:
	void AssignAttackLight();

	void AssignAttackHeavy();

	Input* input_;

	std::unique_ptr<AttackICommand> light = nullptr;
	std::unique_ptr<AttackICommand> heavy = nullptr;
};

