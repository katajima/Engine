#include"Player.h"

// 攻撃更新
void Player::Attack()
{
	//workAttack.parameter++;
	workAttack.parameter += MyGame::GameTime();

	float t = static_cast<float>(workAttack.parameter) / workAttack.attackAll.max_t;
	weapon_->GetObject3D().SetIsDraw(true);
	float k = 0.5f;
	switch (workAttack.type)
	{
	case AttackTypePlay::kNormal:
		if (workAttack.parameter >= 5.0f / 60) {
			effect_->SetIsTrail(true);

		}
		if (workAttack.comboIndex == 0) {
			if (workAttack.parameter >= 1.0f / 60) {
				k = 0.1f;
				weapon_->GetObject3D().worldtransform_.rotate_.x += DegreesToRadians(8 * 60) * MyGame::GameTime();

			}
		}if (workAttack.comboIndex == 1) {
			if (workAttack.parameter >= 1.0f / 60) {
				k = 0.3f;
				weapon_->GetObject3D().worldtransform_.rotate_.x += DegreesToRadians(8 * 60) * MyGame::GameTime();
			}
		}if (workAttack.comboIndex == 2) {
			if (workAttack.parameter >= 1.0f / 60) {
				k = 0.4f;
				weapon_->GetObject3D().worldtransform_.rotate_.x += DegreesToRadians(16 * 60) * MyGame::GameTime();
			}
		}if (workAttack.comboIndex == 3) {
			if (workAttack.parameter >= 1.0f / 60) {
				k = 1.5f;
				weapon_->GetObject3D().worldtransform_.rotate_.x += DegreesToRadians(16 * 60) * MyGame::GameTime();
			}
		}


		if (workAttack.parameter <= 5.0f / 60) {

			Vector3 move(0, 0, k);
			// 速度ベクトルを自機の向きに合わせて回転させる
			move = TransformNormal(move, objectBase_.worldtransform_.worldMat_);

			objectBase_.worldtransform_.translate_ += move;
		}
		break;
	}
}

// 攻撃キー入力
void Player::AttackKey()
{
	if (input_->IsControllerConnected()) {
		workAttack.key.IsNormalAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
		if (bulletSpecial_->GetIsSpecial()) {
			workAttack.key.IsSpecialAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB);
		}
		if (rangeBombingSpecial_->GetIsSpecial()) {
			workAttack.key.IsSpecialAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB);
		}


		if (workAttack.key.IsNormalAttack) {
			workAttack.key.IsAttack = true;

		}

	}

}

// コンボするのか
void Player::SetAttackCombo(WrokAttack& work)
{
	//  既定の時間経過で通常行動に戻る
	work.attackAll.t += MyGame::GameTime();
	if (work.attackAll.t >= work.attackAll.max_t) {
		// コンボ継続なら次のコンボに進む
		if (work.comboNext) {

			work.attackAll.t = 0;

			workAttack.parameter = 0;
			// 各パーツの角度などを次のコンボ用に初期化

			work.comboIndex++;

			// 方向
			Move();
			// 攻撃タイプ
			AttackTypes();
			// 攻撃タイプによって初期化
			AttackTypeInit(work.comboIndex);


			// コンボフラグをリセット
			work.comboNext = false;
			work.key.IsAttack = false;
			effect_->SetIsTrail(false);
			weapon_->SetColliderHistoryClear();
		}
		else {
			effect_->SetIsTrail(false);
			weapon_->GetObject3D().SetIsDraw(false);
			behaviorRequest_ = Behavior::kRoot;
			
		}
	}
	else {
		// コンボ上限に達していない
		if (workAttack.comboIndex < ComboNum - 1) {
			if (workAttack.key.IsAttack) {
				// 攻撃ボタンをトリガーしたら
					// コンボ有効
				workAttack.comboNext = true;


			}
		}

	}

}

// 攻撃位置初期化
void Player::AttackTypeInit(int comboIndex)
{
	objectBase_.Update();
	objectReticle_->Update();
	workAttack.pos.str = objectBase_.GetWorldPosition();
	weapon_->ContactRecordClear();

	// スピード
	const float k = 30.0f;

	if (workAttack.typeRequest_) {
		// ふるまいを変更する
		workAttack.type = workAttack.typeRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (workAttack.type)
		{
		case AttackTypePlay::kNormal:


			if (comboIndex == 0) {
				//weapon_->SetRad(2.5f);
				workAttack.attackAll.max_t = 0.3f;
				weapon_->GetObject3D().worldtransform_.rotate_ = DegreesToRadians({ 0,0,0 });
			}
			if (comboIndex == 1) {
				//weapon_->SetRad(2.5f);
				workAttack.attackAll.max_t = 0.3f;
				weapon_->GetObject3D().worldtransform_.rotate_ = DegreesToRadians({ 0,0,90 });
			}
			if (comboIndex == 2) {
				//weapon_->SetRad(3.5f);
				workAttack.attackAll.max_t = 0.3f;
				weapon_->GetObject3D().worldtransform_.rotate_ = DegreesToRadians({ 0,0,-90 });
			}
			if (comboIndex == 3) {
				//weapon_->SetRad(4.5f);
				workAttack.attackAll.max_t = 0.3f;
				weapon_->GetObject3D().worldtransform_.rotate_ = DegreesToRadians({ 0,0,90 });
			}
			break;
		}
		// ふるまいリクエストリセット
		workAttack.typeRequest_ = std::nullopt;
	}
}

// 攻撃タイプ判別
void Player::AttackTypes()
{
	if (workAttack.key.IsAttack) {
		if (workAttack.key.IsNormalAttack) {
			workAttack.typeRequest_ = AttackTypePlay::kNormal;
		}
	}
}
