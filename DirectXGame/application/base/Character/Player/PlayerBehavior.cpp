#include"Player.h"

#include"DirectXGame/application/base/BaseClass/Bullet/BulletManager.h"

void Player::BehaviorRootInitialize()
{
	weapon_->GetTimer().t = 0.0f;
	
	effect_->SetIsTrail(false);
}

void Player::BehaviorRootUpdate()
{
	

	AttackKey();

	// ジャンプ
	Jump();

	// 移動
	Move();

	recastTime += MyGame::GameTime();
	if (workAttack.key.IsAttack) {
		if (recastTime >= MaxRecastTime) {
			basicbehaviorRequest_ = BasicBehavior::kAttack;
		}
	}
	if (special_->GetIsSpecial()) {
		if (workAttack.key.IsSpecialAttack) {
			if (recastTime >= MaxRecastTime) {
				basicbehaviorRequest_ = BasicBehavior::kSpecialAttack;			
			}
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	weapon_->ResetCurrentTime();
	weapon_->SetTime(0, 1.0f, 0.0f);
	
	AttackTypes();

	AttackTypeInit(0);

	
}

void Player::BehaviorAttackUpdate()
{
	weapon_->SetIsCollider(CollisionTag::PlayerAttack,true);
	AttackKey();

	AttackTypes();


	// コンボ段階によってモーションを分岐
	Attack();

	SetAttackCombo(workAttack);
}

void Player::BehaviorDieInitialize()
{
	special_->SetPhese(0);
	special_->SetGauge(0);	
}

void Player::BehaviorDieUpdate()
{
	Velocity() = {};
	AttackKey();
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
