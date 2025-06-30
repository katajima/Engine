#include"Player.h"

#include "DirectXGame/application/base/Bullet/BulletManager.h"

void Player::BehaviorRootInitialize()
{
	workAttack.parameter = 0;
	
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
	if (bulletSpecial_->GetIsSpecial()) {
		if (workAttack.key.IsSpecialAttack) {
			if (recastTime >= MaxRecastTime) {
				basicbehaviorRequest_ = BasicBehavior::kSpecialAttack;
			}
		}
	}

	if (rangeBombingSpecial_->GetIsSpecial()) {
		if (workAttack.key.IsSpecialAttack) {
			if (recastTime >= MaxRecastTime) {
				basicbehaviorRequest_ = BasicBehavior::kSpecialAttack;			
			}
		}
	}



}

void Player::BehaviorAttackInitialize()
{
	workAttack.attackAll.t = 0;
	workAttack.attackAll.max_t = 1;
	workAttack.comboIndex = 0;
	
	AttackTypes();

	AttackTypeInit(workAttack.comboIndex);

	
}

void Player::BehaviorAttackUpdate()
{
	weapon_->SetIsCollider(true);
	AttackKey();

	AttackTypes();


	// コンボ段階によってモーションを分岐
	Attack();

	SetAttackCombo(workAttack);
	
	float k = 2.5f;
	Vector3 move(0, 0, k);
	// 速度ベクトルを自機の向きに合わせて回転させる
	move = TransformNormal(move, weapon_->GetObject3D().worldtransform_.worldMat_);

}

void Player::BehaviorDieInitialize()
{
	bulletSpecial_->SetPhese(0);
	bulletSpecial_->SetGauge(0);
	rangeBombingSpecial_->SetPhese(0);
	rangeBombingSpecial_->SetGauge(0);
	
}

void Player::BehaviorDieUpdate()
{
	Velocity() = {};
	AttackKey();
	int time = 0;

	ui_->SetIsTextRB(false);

	
	rangeBombingSpecial_->InAction(followCamera_, bulletManager_, rangeBombingPos, reticleRad_);


	objectReticle_->SetIsDraw(false);
	if (rangeBombingSpecial_->GetPhese() == 0) {
		Move();
		ui_->SetIsTextRB(true);
		objectReticle_->SetIsDraw(true);
		rangeBombingPos = objectReticle_->worldtransform_.worldMat_.GetWorldPosition();
	}
	if (rangeBombingSpecial_->GetPhese() == 2) {
		basicbehaviorRequest_ = BasicBehavior::kRoot;
	}


	




	objectBase_->Update();
}
