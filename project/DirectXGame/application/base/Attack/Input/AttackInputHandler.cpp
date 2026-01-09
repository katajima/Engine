#include "AttackInputHandler.h"  
#include "DirectXGame/application/base/Character/base/BaseCharacter.h"
#include "DirectXGame/application/base/Weapon/base/BaseWeapon.h"
#include "DirectXGame/engine/input/Input.h"


AttackICommand::~AttackICommand() 
{  
    // デストラクタの定義  
}


void AttackLight::Exec(BaseCharacter& character) 
{
    character.GetAttackController()->GetComboSystem()->GetComboStateMachine()->HandleInput(AttackInput::Light);
}
void AttackHeavy::Exec(BaseCharacter& character) 
{
    character.GetAttackController()->GetComboSystem()->GetComboStateMachine()->HandleInput(AttackInput::Heavy);
}




AttackICommand* AttackInputHandler::HandleInput()
{
	if (input_->IsControllerConnected()) {
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B)) {
			return light.get();
		}
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Y)) {
			return heavy.get();
		}
	}
	return nullptr;
}

void AttackInputHandler::AssignAttack()
{
	AssignAttackLight();
	AssignAttackHeavy();
}



void AttackInputHandler::AssignAttackLight()
{
	this->light = std::make_unique<AttackLight>();
}

void AttackInputHandler::AssignAttackHeavy()
{
	this->heavy = std::make_unique<AttackHeavy>();
}
