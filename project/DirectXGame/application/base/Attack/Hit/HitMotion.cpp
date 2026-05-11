#include "HitMotion.h"

#pragma region DamageData


void DamageMotion::Update(float dt) {

	damageData_.Update(dt);

	if (damageData_.IsFinish()) isPlaying_ = false;
	else {

	}
}

#pragma endregion // ダメージモーション
