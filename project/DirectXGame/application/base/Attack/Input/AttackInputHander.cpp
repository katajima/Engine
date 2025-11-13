#include "AttackInputHander.h"  
#include "DirectXGame/application/base/Character/base/BaseCharacter.h"
#include "DirectXGame/application/base/Weapon/base/BaseWeapon.h"
#include "DirectXGame/engine/input/Input.h"


AttackICommand::~AttackICommand() 
{  
    // デストラクタの定義  
}


void AttackLight::Exec(BaseCharacter& character) 
{
    character.GetWeapon()->GetComboStateMachine()->HandleInput(AttackInput::Light);
}
void AttackHeavy::Exec(BaseCharacter& character) 
{
    character.GetWeapon()->GetComboStateMachine()->HandleInput(AttackInput::Heavy);
}




AttackICommand* AttackInputHander::HandleInput()
{
	if (input_->IsControllerConnected()) {
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B)) {
			return light.get();
		}
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B)) {
			return heavy.get();
		}
	}
	return nullptr;
}

void AttackInputHander::AssignAttack()
{
	AssignAttackLight();
	AssignAttackHeavy();
}



void AttackInputHander::AssignAttackLight()
{
	this->light = std::make_unique<AttackLight>();
}

void AttackInputHander::AssignAttackHeavy()
{
	this->heavy = std::make_unique<AttackHeavy>();
}
