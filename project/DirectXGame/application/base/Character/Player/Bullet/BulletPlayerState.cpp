#include "BulletPlayerState.h"
#include "BulletPlayer.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/application/base/Bullet/Base/BulletManager.h>

#pragma region Idle

// 更新
void BulletPlayerStateIdle::Update() {
	Input* input = character_->GetInput();
	//BaseSpecial* special = character_->GetSpecial();

	// 武器描画 
	
	// ゲームパッドが繋いであるなら
	if (input->IsControllerConnected()) {

		// 必殺技が使えるようになったら
		//if (character_->GetSpecial()->GetIsSpecial()) {
		////	special->SetIsSpecialAttack(input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB));
		//}

		// スキル発動
		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_X)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Skill);
			return;
		}

		// 防御
		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Defense);
			return;
		}

	}

#ifdef _DEBUG

	// デバッグ用
	if (input->IsTriggerKey(DIK_Z)) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
		return;
	}
#endif // _DEBUG

	// 必殺技移行
	/*if (special->GetIsSpecial()) {
		if (special->GetIsSpecialAttack()) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
			return;
		}
	}*/

	// 移動したら
	if (input->GetGamePadLeftStick().Length() != 0) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}
};

// 終了
void BulletPlayerStateIdle::Exit() {
};

// 初期化
void BulletPlayerStateIdle::Enter() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
	anima->SetAnimetion("Idle1", 0.1f);
};

#pragma endregion // 待機

#pragma region Move

void BulletPlayerStateMove::Update()
{
	Input* input = character_->GetInput();
	//BaseSpecial* special = character_->GetSpecial();

	

	if (input->IsControllerConnected()) {

		/*if (character_->GetSpecial()->GetIsSpecial()) {
			special->SetIsSpecialAttack(input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB));
		}*/

		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_X)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Skill);
			return;
		}

		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Defense);
			return;
		}

	}


	if (input->IsTriggerKey(DIK_Z)) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
		return;
	}


	/*if (special->GetIsSpecial()) {
		if (special->GetIsSpecialAttack()) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
			return;
		}
	}*/

	// 止まったら
	if (input->GetGamePadLeftStick().Length() == 0) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);
		return;
	}
}

void BulletPlayerStateMove::Exit()
{

}

void BulletPlayerStateMove::Enter()
{
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
	anima->SetAnimetion("Walk", 0.1f);
}

#pragma endregion // 移動

#pragma region Jump

// 更新
void BulletPlayerStateJump::Update() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	Input* input = character_->GetInput();

	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定


	// ジャンプ出来るか
	bool isJamp = character_->GetMoveComponent()->GetIsJump();
	bool isTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_Y);
	bool isAlive = character_->GetAlive();

#ifdef _DEBUG
	//ImGui::


#endif // _DEBUG



	// キャラクターが生きていてジャンプ回数が残っていて着地状態じゃないのなら
	if (isAlive && isJamp && isTrigger) {






		character_->GetObjectComponent()->GetRigidBodyComponent()->Velocity().y = 0;
		character_->GetMoveComponent()->DecrementJumpCount(); // ジャンプ回数減少

		// 着地状態なら
		if (character_->GetMoveComponent()->GetIsLanding()) {

		}
		else {
			character_->GetObjectComponent()->GetRigidBodyComponent()->AddForce({ 0,character_->GetCharacterParameterComponent().parameters_.jampPower * 2,0 });
		}
		character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimetion("JumpStrat1", 0.05f);
	}
	else {
		// 着地状態なら
		if (character_->GetMoveComponent()->GetIsLanding()) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}
	}


	// 降下しているならアニメーションを変える
	if (character_->GetObjectComponent()->GetRigidBodyComponent()->Velocity().y <= 0.0f) {
		anima->SetIsLoop(true);
		anima->SetAnimetion("Fall", 0.1f);
	}
	else {	// 上昇しているならアニメーションを変える
		anima->SetIsLoop(false);
		anima->SetAnimetion("JumpStrat1", 0.05f);
	}







}

// 終了
void BulletPlayerStateJump::Exit() {
	character_->GetMoveComponent()->Velocity() = { 0,0,0 };
}
// 初期化
void BulletPlayerStateJump::Enter() {


}

#pragma endregion // ジャンプ

#pragma region Attack

void BulletPlayerStateAttack::Update()
{
	//BaseWeapon* weapon = character_->GetWeapon();

	//weapon->GetComboStateMachine()->Update(character_->GetTime());



	character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
	return;
}

void BulletPlayerStateAttack::Exit()
{
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	// 武器
	character_->GetWeapon()->GetComboStateMachine()->HandleInput(AttackInput::Light);
	character_->GetWeapon()->GetObject3D()->SetIsDraw(false);
	character_->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);

	// アニメーション
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
}

void BulletPlayerStateAttack::Enter()
{
	//BaseWeapon* weapon = character_->GetWeapon();

	// 武器
	//weapon->GetComboStateMachine()->Update(character_->GetTime());
	//weapon->GetObject3D()->SetIsDraw(true);
	//weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
	//weapon->GetColliderComponent()->contactRecord_.Clear();
}

#pragma endregion // 攻撃

#pragma region MyRegion



void BulletPlayerStateSpecial::Update()
{
	//BaseSpecial* special = character_->GetSpecial();
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	character_->Velocity() = {};
	int time = 0;
	player->GetPlayerUI()->SetIsTextRB(false);
	
}

void BulletPlayerStateSpecial::Exit()
{
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	player->GetPlayerUI()->SetIsTextRB(false);
}

void BulletPlayerStateSpecial::Enter()
{
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	player->GetSpecial()->SetPhese(0);
	player->GetSpecial()->SetGauge(0);
	player->GetWeapon()->GetObject3D()->SetIsDraw(false);
}

#pragma endregion // 必殺技

#pragma region Skill
// 更新
void BulletPlayerStateSkill::Update() {

	timer_ += character_->GetTime();
	if (changeTimer_ <= timer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}

};

// 終了
void BulletPlayerStateSkill::Exit() {

};
// 初期化
void BulletPlayerStateSkill::Enter() {
	//BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	//timer_ = 0.0f;
	//player->GetBulletManager()->GenerateBullet(BulletManager::BulletType::kPlayerStan, player->GetWorldTransform().worldMat_.GetWorldPosition());
};
#pragma endregion // スキル

#pragma region Defense

// 更新
void BulletPlayerStateDefense::Update() {
	Input* input = character_->GetInput();


	if (input->IsControllerConnected()) {
		if (input->IsGamePadPressed(GamePadButton::GAMEPAD_A) && isDifense_) {
			isDifense_ = true;

			character_->GetCombatStatComponent()->damageReduction_ = 0.75;

			if (character_->GetCharacterParameterComponent().IsGetStamina()) {
				character_->GetCharacterParameterComponent().Stamina().rateFluctuation = -5.0f;
			}
			else {
				isDifense_ = false;
			}

		}
		else {
			character_->GetCharacterParameterComponent().Stamina().rateFluctuation = 5.0f;
			isDifense_ = false;
			timer_ += character_->GetTime();
			character_->GetCombatStatComponent()->damageReduction_ = 0.0f;
		}
	}



	if (timer_ >= defenseTimer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}

};

// 終了
void BulletPlayerStateDefense::Exit() {
	isDifense_ = false;
	character_->GetCombatStatComponent()->damageReduction_ = 0.0f;
	character_->GetCharacterParameterComponent().Stamina().rateFluctuation = 5.0f;
};
// 初期化
void BulletPlayerStateDefense::Enter() {
	isDifense_ = true;
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetIsLoop(false);		// アニメーションをループさせるか
	anima->SetStratAnimeTime();		// アニメーション時間を初期化
	anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定
	anima->SetAnimetion("Defense1", 0.10f);
};

#pragma endregion // 防御

#pragma region Fainting

// 更新
void BulletPlayerStateFainting::Update() {
	Input* input = character_->GetInput();
	Vector2 left = input->GetGamePadLeftStick();

	timer_ += character_->GetTime();

	if (prevleftStick != left) {
		Vector2 sub = prevleftStick - left;
		float length = sub.Length();
		float subtime = 1.0f;

		if (length >= 10.0f) {
			subtime = 2.0f;
		}

		timer_ += character_->GetTime() * subtime;
	}


	if (timer_ >= faintingTimer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}


	prevleftStick = left;
}

// 終了
void BulletPlayerStateFainting::Exit() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetIsLoop(true);			// アニメーションをループさせるか
	anima->SetStratAnimeTime();		// アニメーション時間を初期化
	anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定
};
// 初期化
void BulletPlayerStateFainting::Enter() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetIsLoop(true);			// アニメーションをループさせるか
	anima->SetStratAnimeTime();		// アニメーション時間を初期化
	anima->SetAnimationSpeed(0.75f); // アニメーションスピード設定
	anima->SetAnimetion("Stan1", 0.10f);
	timer_ = 0;		// タイマーを0に設定


	Input* input = character_->GetInput();
	prevleftStick = input->GetGamePadLeftStick();
};

#pragma endregion // 気絶