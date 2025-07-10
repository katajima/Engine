#include "BaseWeapon.h"
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"



void BaseWeapon::SetAttackCombo(float deltaTime) {
	//  既定の時間経過で通常行動に戻る
	AddCurrentTime(deltaTime);
	if (GetCurrentTimer() >= GetAllTime()) {
		// コンボ継続なら次のコンボに進む
		if (IsComboNext()) {

			ResetCurrentTime();

			GetTimer().t = 0;
			GetTimer().maxT = GetAllTime();

			// 各パーツの角度などを次のコンボ用に初期化

			IncrementCurrentComboCount();

			// 攻撃タイプによって初期化
			AttackTypeInit(GetCurrentComboCount());

			// 方向
			character->Move();
			// 攻撃タイプ
			KeyAttackTypes(character->GetSituation().isJumping);

			isState = true;
			// コンボフラグをリセット
			SetIsComboNext(false);
			SetIsAttack(false);
			ColliderHistoryClear();
		}
		else {
			ResetCurrentComboCount();
			isState = false;
			isAttack = false;
			data_.recastTime = 0.0f;
			// 通常行動へ移行
			character->SetRequest(BasicBehavior::kRoot);
		}
	}
	else {
		// コンボ上限に達していない
		if (GetCurrentComboCount() < GetComboMaxCount() - 1) {
			if (IsAttack()) {
				// コンボ有効
				SetIsComboNext(true);
			}
		}
	}
}


void BaseWeapon::AttackTypeInit(int comboIndex) {
	ColliderHistoryClear();
	if (GetTypeRequest()) {
		// ふるまいを変更する
		ChangeRequest();
		ResetCurrentTime();
		// 各ふるまいごとの初期化を実行
		switch (GetAttackTypePlay())
		{
		case AttackTypePlay::kNormal:

			if (comboIndex == 0) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
			}
			if (comboIndex == 1) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
			}
			if (comboIndex == 2) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,-90 });
			}
			if (comboIndex == 3) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
			}
			break;
		case AttackTypePlay::kJump:
			if (comboIndex == 0) {
				SetTime(0.0f, 0.4f, 0.1f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
			}
			break;
		}
		// ふるまいリクエストリセット
		ResetRequest();
	}
}


void BaseWeapon::AttackUpdate() {
	SetIsCollider(CollisionTag::PlayerAttack, true);

	if (input_->IsControllerConnected()) {
		GetAttackKeyFlag().IsNormalAttack = input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
		if (GetAttackKeyFlag().IsNormalAttack) {
			SetIsAttack(true);
		}
	}
	KeyAttackTypes(character->GetSituation().isJumping);


	// コンボ段階によってモーションを分岐
	AttackUpdate(character->GetTime(), character->GetObject3D()->worldtransform_);

	// コンボ攻撃
	SetAttackCombo(character->GetTime());
}

void BaseWeapon::AttackUpdate(float deltaTime, WorldTransform& worldTransform) {
	GetTimer().Update(deltaTime);

	GetObject3D()->SetIsDraw(true);
	switch (GetAttackTypePlay())
	{
	case AttackTypePlay::kNormal:
		if (GetCurrentComboCount() == 0) {
			if (GetComboMotionData().isStartup) {
				SetMovementSpeedMultiplier(0.01f);
				GetWorldTransform().rotate_.x += DegreesToRadians(1 * 60) * deltaTime;
			}
			if (GetComboMotionData().isAttackAnimation) {
				SetMovementSpeedMultiplier(0.1f);
				GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;
			}
			if (GetComboMotionData().isRecovery) {
				SetMovementSpeedMultiplier(1.05f);
				GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;
			}
		}if (GetCurrentComboCount() == 1) {
			if (GetTimer().t >= GetStartupTime()) {
				SetMovementSpeedMultiplier(0.3f);
				GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;
			}
		}if (GetCurrentComboCount() == 2) {
			if (GetTimer().t >= GetStartupTime()) {
				SetMovementSpeedMultiplier(0.4f);
				GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * deltaTime;
			}
		}if (GetCurrentComboCount() == 3) {
			if (GetTimer().t >= GetStartupTime()) {
				SetMovementSpeedMultiplier(1.5f);
				GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * deltaTime;
			}
		}


		if (GetTimer().t <= 5.0f / 60) {

			Vector3 move(0, 0, GetMovementSpeedMultiplier());
			// 速度ベクトルを自機の向きに合わせて回転させる
			move = TransformNormal(move, worldTransform.worldMat_);

			worldTransform.translate_ += move;
		}
		break;
	case AttackTypePlay::kJump:
		if (GetTimer().t >= 1.0f / 60) {
			GetWorldTransform().rotate_.x += DegreesToRadians(16 * 180) * deltaTime;
			SetMovementSpeedMultiplier(0.2f);
			Vector3 move(0, 0, GetMovementSpeedMultiplier());
			// 速度ベクトルを自機の向きに合わせて回転させる
			move = TransformNormal(move, worldTransform.worldMat_);
			worldTransform.translate_ += move;
		}
		break;
	}
}