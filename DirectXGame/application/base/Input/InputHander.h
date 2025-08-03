#pragma once
#include<memory>


class BaseCharacter;
class ICommand 
{
public:
	virtual ~ICommand();
	virtual void Exec(BaseCharacter& character) = 0;

};

class MoveCommand :public ICommand 
{
public:
	void Exec(BaseCharacter& character) override;
};

class JampCommand : public ICommand
{
public:
	void Exec(BaseCharacter& character) override;
};

class AttackCommand : public ICommand
{
public:
	void Exec(BaseCharacter& character) override;
};

class Input;
class InputHander 
{
public:
	void SetInput(Input* input) { input_ = input; };

	ICommand* HandleInput();

	void AssignMoveCommandPad();
	void AssignJampCommandPad();
	void AssignAttackCommandPad();

private:
	Input* input_;

	std::unique_ptr<ICommand> movePad;
	std::unique_ptr<ICommand> jampPad;
	std::unique_ptr<ICommand> attackPad;

};