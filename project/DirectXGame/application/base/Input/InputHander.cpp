#include "InputHander.h"
#include "DirectXGame/application/base/Character/base/BaseCharacter.h"
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
	character.Attack();
}
void HeavyAttackCommand::Exec(BaseCharacter& character)
{
	character.HeavyAttack();
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
void InputHander::AssignHeavyAttackCommandPad()
{
	this->attackHeavyPad = std::make_unique<HeavyAttackCommand>();
}



ICommand* InputHander::HandleInput()
{
	// ゲームパッドがつながっているなら
	if (input_->IsControllerConnected()) {
		// ジャンプコマンド
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
			return jampPad.get();
		}
		// 攻撃コマンド(弱攻撃)
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B)) {
			return attackPad.get();
		}
		// 攻撃コマンド(強攻撃)
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Y)) {
			return attackHeavyPad.get();
		}
		// 移動コマンド
		if (input_->GetGamePadLeftStick().Length() != 0) {
			return movePad.get();
		}
		
	}
	return nullptr;
}