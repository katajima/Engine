#include "EffectModule.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>



void DamageModule::Effect(BaseCharacter* character){
	character->AddDamage(damageAmount);
}

void KnockbackModule::Effect(BaseCharacter* character) {
	character->GetHitMotionComponent()->SetIsKnockback(isknockback_);
	character->GetHitMotionComponent()->SetKnockbackData(knockback_);
	character->GetHitMotionComponent()->SetKnockbackTime(knockbackTime_);
}

void ChangeStateModule::Effect(BaseCharacter* character) {
	character->GetCharacterStateMachine()->ChangeState(state_); // ステート設定
};