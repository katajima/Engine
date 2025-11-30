#include "HitCounter.h"
#include <DirectXGame/application/base/Character/Base/CharacterData.h>

#pragma region CombatStat

float CombatStat::GetEffectiveAttack() const {
	return baseParams_->GetStrength() * data_.attackMultiplier_;
}

float CombatStat::GetEffectiveDefense() const {
	return baseParams_->GetDefense() * data_.defenseMultiplier_;
}



#pragma endregion


#pragma region HitCounter

void HitCounter::Update(float dt) {
	hitData_.Update(dt);
};

#pragma endregion


