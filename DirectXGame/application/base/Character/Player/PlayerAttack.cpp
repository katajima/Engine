#include"Player.h"
#include "DirectXGame/application/base/BaseClass/Weapon/Weapons.h"


// 攻撃更新
void Player::Attack()
{
	weapon_->GetTimer().Update(MyGame::GameTime());

	weapon_->GetObject3D()->SetIsDraw(true);
	switch (weapon_->GetAttackTypePlay())
	{
	case AttackTypePlay::kNormal:
		if (weapon_->GetCurrentComboCount() == 0) {
			if (weapon_->GetTimer().t >= weapon_->GetStartupTime()) {
				weapon_->SetMovementSpeedMultiplier(0.1f);
				weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * MyGame::GameTime();

			}
		}if (weapon_->GetCurrentComboCount() == 1) {
			if (weapon_->GetTimer().t >= weapon_->GetStartupTime()) {
				weapon_->SetMovementSpeedMultiplier(0.3f);
				weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * MyGame::GameTime();
			}
		}if (weapon_->GetCurrentComboCount() == 2) {
			if (weapon_->GetTimer().t >= weapon_->GetStartupTime()) {
				weapon_->SetMovementSpeedMultiplier(0.4f);
				weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * MyGame::GameTime();
			}
		}if (weapon_->GetCurrentComboCount() == 3) {
			if (weapon_->GetTimer().t >= weapon_->GetStartupTime()) {
				weapon_->SetMovementSpeedMultiplier(1.5f);
				weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * MyGame::GameTime();
			}
		}


		if (weapon_->GetTimer().t <= 5.0f / 60) {

			Vector3 move(0, 0, weapon_->GetMovementSpeedMultiplier());
			// 速度ベクトルを自機の向きに合わせて回転させる
			move = TransformNormal(move, objectBase_->worldtransform_.worldMat_);

			objectBase_->worldtransform_.translate_ += move;
		}
		break;
	case AttackTypePlay::kJump:
		if (weapon_->GetTimer().t >= 1.0f / 60) {
			weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(16 * 180) * MyGame::GameTime();
			weapon_->SetMovementSpeedMultiplier(0.2f);
			Vector3 move(0, 0, weapon_->GetMovementSpeedMultiplier());
			// 速度ベクトルを自機の向きに合わせて回転させる
			move = TransformNormal(move, objectBase_->worldtransform_.worldMat_);
			objectBase_->worldtransform_.translate_ += move;
		}
		break;
	}
}

// 攻撃キー入力
void Player::AttackKey()
{
	if (input_->IsControllerConnected()) {
		workAttack.key.IsNormalAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
		if (special_->GetIsSpecial()) {
			workAttack.key.IsSpecialAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB);
		}
		if (workAttack.key.IsNormalAttack) {
			weapon_->SetIsAttack(true);
		}
	}
}

// コンボするのか
void Player::SetAttackCombo()
{
	if (weapon_->GetCurrentTimer() >= weapon_->GetAllTime()) {
		// コンボ継続なら次のコンボに進む
		if (weapon_->IsComboNext()) {
			// 方向
			Move();
			// 攻撃タイプ
			AttackTypes();
		}
		else {
			basicbehaviorRequest_ = BasicBehavior::kRoot;
		}
	}
	// コンボ攻撃
	weapon_->SetAttackCombo(MyGame::GameTime());
}


// 攻撃タイプ判別
void Player::AttackTypes()
{
	if (weapon_->IsAttack()) {
		if (workAttack.key.IsNormalAttack) {
			if (GetSituation().isJumping && weapon_->GetCurrentComboCount() == 0) {
				weapon_->SetRequest(AttackTypePlay::kJump);
			}
			else {
				weapon_->SetRequest(AttackTypePlay::kNormal);
			}
		}
	}
}
