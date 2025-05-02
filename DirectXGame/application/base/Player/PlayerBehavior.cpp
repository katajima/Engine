#include"Player.h"

#include "DirectXGame/application/base/Bullet/BulletManager.h"

void Player::BehaviorRootInitialize()
{
	effect_->GetDashEmitter()->SetIsEmit(false);
	workAttack.parameter = 0;
	
	effect_->SetIsTrail(false);
}

void Player::BehaviorRootUpdate()
{
	

	AttackKey();


	// 移動
	Move();

	

	recastTime += MyGame::GameTime();
	if (workAttack.key.IsAttack) {
		if (recastTime >= MaxRecastTime) {
			behaviorRequest_ = Behavior::kAttack;
		}
	}
	if (bulletSpecial_->GetIsSpecial()) {
		if (workAttack.key.IsSpecialAttack) {
			if (recastTime >= MaxRecastTime) {
				behaviorRequest_ = Behavior::kDie;
				
			}
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	workAttack.attackAll.t = 0;
	workAttack.attackAll.max_t = 1;
	workAttack.comboIndex = 0;
	

	effect_->GetDashEmitter()->SetIsEmit(false);
	AttackTypes();

	AttackTypeInit(workAttack.comboIndex);

	
}

void Player::BehaviorAttackUpdate()
{
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
	effect_->GetDashEmitter()->SetIsEmit(false);
	bulletSpecial_->SetPhese(0);
	bulletSpecial_->SetGauge(0);

}

void Player::BehaviorDieUpdate()
{
	velocity_ = {};
	AttackKey();
	int time = 0;

	ui_->SetIsTextRB(false);

	bulletSpecial_->InAction(followCamera_, bulletManager_, lockedOnEnemies);

	if (bulletSpecial_->GetPhese() == 0) {
		Move();
		ui_->SetIsTextRB(true);
	}
	if (bulletSpecial_->GetPhese() == 2) {
		behaviorRequest_ = Behavior::kRoot;
	}


	




	objectBase_.Update();
}
