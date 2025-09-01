#include "InputHander.h"
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"
#include"DirectXGame/engine/input/Input.h"


ICharacterCommand::~ICharacterCommand()
{
}

void MoveCommand::Exec(BaseCharacter& character)
{
	character.Move();
}
void JampCommand::Exec(BaseCharacter& character)
{
	character.Jump();
}
void AttackCommand::Exec(BaseCharacter& character)
{
	character.Attack();
}


void InputHander::AssignMoveCommandPad()
{
	this->movePad = std::make_unique<MoveCommand>();
}

void InputHander::AssignJampCommandPad()
{
	this->jampPad = std::make_unique<JampCommand>();
}

void InputHander::AssignAttackCommandPad()
{
	this->attackPad = std::make_unique<AttackCommand>();
}



ICharacterCommand* InputHander::HandleInput()
{
	if (input_->IsControllerConnected()) {
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Y)) {
			return jampPad.get();
		}
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B)) {
			return attackPad.get();
		}
		if (input_->GetGamePadLeftStick().Length() != 0) {
			return movePad.get();
		}
		
	}
	return nullptr;
}

ICommand::~ICommand()
{
}

void MenuCommand::Exec()
{
}
