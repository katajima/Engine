#include "InputHander.h"
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"
#include"DirectXGame/engine/input/Input.h"

ICommand::~ICommand()
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
	character.Jump();
}


void InputHander::AssignMoveCommandPad()
{
	ICommand* command = new MoveCommand();
	this->movePad = command;
}

void InputHander::AssignJampCommandPad()
{
	ICommand* command = new JampCommand();
	this->jampPad = command;
}



ICommand* InputHander::HandleInput()
{
	if (input_->IsControllerConnected()) {
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Y)) {
			return jampPad;
		}
		if (input_->GetGamePadLeftStick().Length() != 0) {
			return movePad;
		}
	}
	return nullptr;
}