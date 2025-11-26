#include "EffectModule.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>



void DamageModule::Effect(BaseCharacter* character){
	// ダメージ追加
	character->AddDamage(damageAmount);
}

void KnockbackModule::Effect(BaseCharacter* character) {

	// ノックバックのデータをそれぞれ送る
	//character->GetHitMotionComponent()->SetIsKnockback(isknockback_);	// ノックバックするか
	//character->GetHitMotionComponent()->SetKnockbackData(knockback_);	// ノックバックデータ
	//character->GetHitMotionComponent()->SetKnockbackTime(knockbackTime_);// ノックバック時間
}

void ChangeStateModule::Effect(BaseCharacter* character) {
	character->GetCharacterStateMachine()->ChangeState(state_); // ステート設定
};