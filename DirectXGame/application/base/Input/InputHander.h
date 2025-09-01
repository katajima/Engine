#pragma once
#include<memory>


class BaseCharacter;
class ICharacterCommand
{
public:
	virtual ~ICharacterCommand();
	virtual void Exec(BaseCharacter& character) = 0;

};
class ICommand
{
public:
	virtual ~ICommand();
	virtual void Exec() = 0;
};

class MoveCommand :public ICharacterCommand
{
public:
	void Exec(BaseCharacter& character) override;
};

class JampCommand : public ICharacterCommand
{
public:
	void Exec(BaseCharacter& character) override;
};

class AttackCommand : public ICharacterCommand
{
public:
	void Exec(BaseCharacter& character) override;
};

class MenuCommand : public ICommand {
public:
	void Exec() override;
};

class Input;
class InputHander 
{
public:
	void SetInput(Input* input) { input_ = input; };

	ICharacterCommand* HandleInput();
	//ICommand* HandleInput();
	void AssignMoveCommandPad();
	void AssignJampCommandPad();
	void AssignAttackCommandPad();

private:
	Input* input_;

	std::unique_ptr<ICharacterCommand> movePad;
	std::unique_ptr<ICharacterCommand> jampPad;
	std::unique_ptr<ICharacterCommand> attackPad;

};