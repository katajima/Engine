#include"Player.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/BaseClass/Bullet/BulletManager.h"

void Player::BehaviorRootInitialize()
{
	weapon_->GetTimer().t = 0.0f;
	effect_->SetIsTrail(false);
}

void Player::BehaviorRootUpdate()
{
	// 
	if (input_->IsControllerConnected()) {

		if (special_->GetIsSpecial()) {
			IsSpecialAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB);
		}

		weapon_->GetAttackKeyFlag().IsNormalAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
		if (weapon_->GetAttackKeyFlag().IsNormalAttack) {
			weapon_->SetIsAttack(true);
		}
	}

	// ジャンプ
	Jump();

	// 移動
	Move();

	recastTime += MyGame::GameTime();
	if (weapon_->IsAttack()) {
		if (recastTime >= MaxRecastTime) {
			basicbehaviorRequest_ = BasicBehavior::kAttack;
		}
	}
	if (special_->GetIsSpecial()) {
		if (IsSpecialAttack) {
			if (recastTime >= MaxRecastTime) {
				basicbehaviorRequest_ = BasicBehavior::kSpecialAttack;
			}
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	weapon_->KeyAttackTypes(Situations().isJumping);
	weapon_->AttackTypeInit(0);
}

void Player::BehaviorAttackUpdate()
{
	// 攻撃処理
	weapon_->AttackUpdate();
}

void Player::BehaviorDieInitialize()
{
	special_->SetPhese(0);
	special_->SetGauge(0);
}

void Player::BehaviorDieUpdate()
{
	Velocity() = {};
	int time = 0;
	ui_->SetIsTextRB(false);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
	rengeSp->InAction(followCamera_, bulletManager_, rangeBombingPos, reticleRad_);
	objectReticle_->SetIsDraw(false);
	if (special_->GetPhese() == 0) {
		Move();
		ui_->SetIsTextRB(true);
		objectReticle_->SetIsDraw(true);
		rangeBombingPos = objectReticle_->worldtransform_.worldMat_.GetWorldPosition();
	}
	if (special_->GetPhese() == 2) {
		basicbehaviorRequest_ = BasicBehavior::kRoot;
	}
	objectBase_->Update();
}
