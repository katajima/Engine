#include "HitMotion.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

#pragma region HitStopMotion

void HitStopMotion::Update(float dt) {
	
	// ヒットストップ時間の更新
	hitStopData_.Update(dt);

	// ヒットストップ終了判定
	if (hitStopData_.IsFinish()) isPlaying_ = false;
}

#pragma endregion // ヒットストップ

#pragma region KnockbackMotion

// 更新
void KnockbackMotion::Update(float dt, ObjectComponent* object) {

	// ノックバック時間の更新
	knockbackData_.Update(dt);

	// ノックバック終了判定
	if(knockbackData_.IsFinish()) isPlaying_ = false;
	else {
		// ノックバック方向にキャラクターを飛ばす
		object->GetWorldTransform().translate_ += knockbackData_.DirectionPower() * dt;
	}
}

#pragma endregion // ノックバックモーション

#pragma region AirStickMotion

// 更新
void AirStickMotion::Update(float dt, ObjectComponent* object) {
	// エアスティック時間の更新
	airStickData_.Update(dt);
	// エアスティック終了判定
	if (airStickData_.IsFinish()) isPlaying_ = false;
	else {
		// エアスティック方向にキャラクターを飛ばす
		//object->GetWorldTransform().translate_ += airStickData_.DirectionPower() * dt;
	}
};

#pragma endregion // エアスティックモーション

#pragma region DamageData


void DamageMotion::Update(float dt) {

	damageData_.Update(dt);

	if (damageData_.IsFinish()) isPlaying_ = false;
	else {

	}
}

#pragma endregion // ダメージモーション
