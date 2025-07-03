#include"Player.h"
#include "DirectXGame/application/base/BaseClass/Weapon/Weapons.h"


// 攻撃更新
void Player::Attack()
{
	weapon_->GetTimer().Update(MyGame::GameTime());

	weapon_->GetObject3D()->SetIsDraw(true);
	switch (workAttack.type)
	{
	case AttackTypePlay::kNormal:
		if (weapon_->GetTimer().t >= 5.0f / 60) {
			effect_->SetIsTrail(true);
		}
		if (weapon_->GetCurrentComboCount() == 0) {
			if (weapon_->GetTimer().t >= 1.0f / 60) {
				weapon_->SetMovementSpeedMultiplier(0.1f);
				weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * MyGame::GameTime();

			}
		}if (weapon_->GetCurrentComboCount() == 1) {
			if (weapon_->GetTimer().t >= 1.0f / 60) {
				weapon_->SetMovementSpeedMultiplier(0.3f);
				weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * MyGame::GameTime();
			}
		}if (weapon_->GetCurrentComboCount() == 2) {
			if (weapon_->GetTimer().t >= 1.0f / 60) {
				weapon_->SetMovementSpeedMultiplier(0.4f);
				weapon_->GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * MyGame::GameTime();
			}
		}if (weapon_->GetCurrentComboCount() == 3) {
			if (weapon_->GetTimer().t >= 1.0f / 60) {
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
	weapon_->AddCurrentTime(MyGame::GameTime());
	if (weapon_->GetCurrentTimer() >= weapon_->GetAllTime()) {
		// コンボ継続なら次のコンボに進む
		if (weapon_->IsComboNext()) {

			weapon_->ResetCurrentTime();

			weapon_->GetTimer().t = 0;
			weapon_->GetTimer().maxT = weapon_->GetAllTime();

			// 各パーツの角度などを次のコンボ用に初期化

			weapon_->IncrementCurrentComboCount();

			// 方向
			Move();
			// 攻撃タイプ
			AttackTypes();
			// 攻撃タイプによって初期化
			AttackTypeInit(weapon_->GetCurrentComboCount());


			// コンボフラグをリセット
			weapon_->SetIsComboNext(false);
			work.key.IsAttack = false;
			effect_->SetIsTrail(false);
			weapon_->ColliderHistoryClear();
		}
		else {
			effect_->SetIsTrail(false);
			weapon_->ResetCurrentComboCount();
			basicbehaviorRequest_ = BasicBehavior::kRoot;	
		}
	}
	else {
		// コンボ上限に達していない
		if (weapon_->GetCurrentComboCount() < weapon_->GetComboMaxCount() - 1) {
			if (workAttack.key.IsAttack) {
				// 攻撃ボタンをトリガーしたら
				// コンボ有効
				weapon_->SetIsComboNext(true);
			}
		}
	}
}

// 攻撃位置初期化
void Player::AttackTypeInit(int comboIndex)
{
	objectBase_->Update();
	objectReticle_->Update();
	weapon_->ColliderHistoryClear();

	if (workAttack.typeRequest_) {
		// ふるまいを変更する
		workAttack.type = workAttack.typeRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (workAttack.type)
		{
		case AttackTypePlay::kNormal:


			if (comboIndex == 0) {
				weapon_->SetTime(0.0f,0.2f,0.1f);
				weapon_->GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
			}
			if (comboIndex == 1) {
				weapon_->SetTime(0.0f, 0.2f, 0.1f);
				weapon_->GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
			}
			if (comboIndex == 2) {
				weapon_->SetTime(0.0f, 0.2f, 0.1f);
				weapon_->GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,-90 });
			}
			if (comboIndex == 3) {
				weapon_->SetTime(0.0f, 0.2f, 0.1f);
				weapon_->GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
			}
			break;
		case AttackTypePlay::kJump:
			if (comboIndex == 0) {
				weapon_->SetTime(0.0f, 0.4f, 0.1f);
				weapon_->GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
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
			if (GetSituation().isJumping && weapon_->GetCurrentComboCount() == 0) {
				workAttack.typeRequest_ = AttackTypePlay::kJump;
			}
			else {
				workAttack.typeRequest_ = AttackTypePlay::kNormal;
			}
		}
	}
}
