#include "BaseWeapon.h"
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"



void BaseWeapon::SetAttackCombo(float deltaTime) {
	//  既定の時間経過で通常行動に戻る
	 data_.motionData.AddCurrentTime(deltaTime);
	if (data_.motionData.currentTime >= data_.motionData.AllTime()) {
		// コンボ継続なら次のコンボに進む
		if (GetComboData().isComboNext) {

			data_.motionData.ResetTime();

			GetTimer().t = 0;
			GetTimer().maxT = data_.motionData.AllTime();

			// 各パーツの角度などを次のコンボ用に初期化

			GetComboData().IncrementCurrentComboCount();

			// 攻撃タイプによって初期化
			AttackTypeInit(GetComboData().GetCurrentComboCount());

			// 方向
			character->Move();
			// 攻撃タイプ
			KeyAttackTypes(character->GetSituation().isJumping);

			attackInput_.SetIsState(true);

			// コンボフラグをリセット
			GetComboData().SetIsComboNext(false);
			attackInput_.SetIsAttack(false);
			ColliderHistoryClear();
		}
		else {
			GetComboData().ResetCurrentComboCount();
			attackInput_.SetIsState(false);
			attackInput_.SetIsAttack(false);

			data_.recastTime = 0.0f;
			// 通常行動へ移行
			character->SetRequest(BasicBehavior::kRoot);
		}
	}
	else {
		// コンボ上限に達していない
		if (GetComboData().currentComboCount < GetComboData().comboMaxCount - 1) {
			if (attackInput_.GetIsAttack()) {
				// コンボ有効
				GetComboData().SetIsComboNext(true);
			}
		}
	}
}


void BaseWeapon::AttackUpdate() {
	SetIsCollider(CollisionTag::PlayerAttack, true);

	if (input_->IsControllerConnected()) {
		attackInput_.GetAttackKeyFlag().IsNormalAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
		if (attackInput_.GetAttackKeyFlag().IsNormalAttack) {
			attackInput_.SetIsAttack(true);
		}
	}
	KeyAttackTypes(character->GetSituation().isJumping);


	// コンボ段階によってモーションを分岐
	AttackUpdate(character->GetTime(), character->GetObject3D()->worldtransform_);

	// コンボ攻撃
	SetAttackCombo(character->GetTime());
}