#include"Player.h"

#include"DirectXGame/application/base/Enemy/Enemy.h"

void Player::BehaviorRootInitialize()
{
	dashEmitter_->SetIsEmit(false);
	workAttack.parameter = 0;
	
	flag33 = false;
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
	if (specialAttack.specialGauge >= specialAttack.max) {
		if (workAttack.key.IsSpecialAttack) {
			if (recastTime >= MaxRecastTime) {
				behaviorRequest_ = Behavior::kDie;
				specialAttack.specialGauge = 0;
			}
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	workAttack.attackAll.t = 0;
	workAttack.attackAll.max_t = 1;
	workAttack.comboIndex = 0;
	

	dashEmitter_->SetIsEmit(false);
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
	dashEmitter_->SetIsEmit(false);
	specialAttack.phese = 0;
	specialAttack.specialGauge = 0;
}

void Player::BehaviorDieUpdate()
{
	velocity_ = {};
	AttackKey();
	int i = 0;
	int time = 0;
	isTextRB_ = false;
	switch (specialAttack.phese)
	{
	case 0:
		specialAttack.time += MyGame::GameTime();
		// 移動
		Move();

		if (specialAttack.time >= 0.5f) {
			if (Input::GetInstance()->IsGamePadTriggered(GamePadButton::GAMEPAD_RB)) {
				specialAttack.phese = 1;
				specialAttack.time = 0;
			}
		}
		index_b = 0;
		isTextRB_ = true;

		break;
	case 1:
		// 弾を発射
		// 移動
		//Move();
		specialAttack.time += MyGame::GameTime();
		time = int(specialAttack.time * 60);
		injectionLeftObj_.worldtransform_.translate_.y = injectionLeftPos_.y;
		injectionRightObj_.worldtransform_.translate_.y = injectionRightPos_.y;
		if (time % 10 == 0) {
			specialAttack.clock *= -1;
			while (index_b < lockedOnEnemies.size())
			{
				auto bullet = std::make_unique<PlayerBullet>();
				bullet->SetIndex(index_b);
				if (specialAttack.clock == 1) {
					followCamera_->GetViewProjection().SetShake(1.3f, {0.2f,0.2f,0.2f});

					bullet->Initialize(injectionLeftObj_.GetWorldPosition(), camera_);
					injectionLeftObj_.worldtransform_.translate_.y -= 0.5f;
				}
				else {
					followCamera_->GetViewProjection().SetShake(1.3f, { 0.2f,0.2f,0.2f });
					bullet->Initialize(injectionRightObj_.GetWorldPosition(), camera_);
					injectionRightObj_.worldtransform_.translate_.y -= 0.5f;
				}
				bullet->SetEnemy(lockedOnEnemies[index_b]);
				bullet->SetPlayer(this);
				//bullet->SetParent(objectBase_.worldtransform_);
				
				

				playerBullet_.push_back(std::move(bullet));

				

				index_b++;
				break;
			}
		}
		
	
			
		

		if (lockedOnEnemies.size() <= playerBullet_.size())
		{
			specialAttack.phese = 2;
		}

		break;
	case 2:
		behaviorRequest_ = Behavior::kRoot;

		break;
	case 3:
		break;
	case 4:
		break;

	}


	




	objectBase_.Update();
}
