#include "BaseCharacter.h"

void Character::BaseCharacter::InitializeBaseAddItem() {
	AddItem("HP", parameterComponent_->parameters->HP.value);
	AddItem("MaxHP", parameterComponent_->parameters->HP.maxValue);
	AddItem("MP", parameterComponent_->parameters->MP.value);
	AddItem("MaxMP", parameterComponent_->parameters->MP.maxValue);
	AddItem("stamina", parameterComponent_->parameters->stamina.value);
	AddItem("MaxStamina", parameterComponent_->parameters->stamina.maxValue);



	parameterComponent_->parameters->HP.value = GetValue<float>("HP");
	parameterComponent_->parameters->HP.maxValue = GetValue<float>("MaxHP");
	parameterComponent_->parameters->MP.value = GetValue<float>("MP");
	parameterComponent_->parameters->MP.maxValue = GetValue<float>("MaxMP");
	parameterComponent_->parameters->stamina.value = GetValue<float>("stamina");
	parameterComponent_->parameters->stamina.maxValue = GetValue<float>("MaxStamina");
}

void Character::BaseCharacter::UpdateBaseGetValue() {}
