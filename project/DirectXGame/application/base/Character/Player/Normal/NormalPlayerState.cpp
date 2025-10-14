#include "NormalPlayerState.h"
#include"Player.h"
#include "DirectXGame/engine/MyGame/MyGame.h"


#pragma region Move

void PlayerStateMove::Update()
{
	AnimationComponent* anima = character_->GetObject3D()->GetAnimationComponent();
	Input* input = character_->GetInput();
	BaseWeapon* weapon = character_->GetWeapon();
	BaseSpecial* special = character_->GetSpecial();

	weapon->GetObject3D()->SetIsDraw(true);

	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);

	if (character_->GetCharacterStateComponent().IsJumping()) {
		if (character_->GetObject3D()->GetRigidBodyComponent()->Velocity().y <= 0.0f) {
			anima->SetAnimetion("Fall", 0.01f);
		}
		else {
			anima->SetAnimetion("Fall", 0.01f);
		}
	}
	else {
		if (character_->GetVelocity().Length() != 0) {

			anima->SetAnimetion("Walk", 0.1f);
		}
		else {
			anima->SetAnimetion("Idle1", 0.1f);
		}
	}


	if (input->IsControllerConnected()) {

		if (character_->GetSpecial()->GetIsSpecial()) {
			special->SetIsSpecialAttack(input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB));
		}

		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_X)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Skill);
		}

		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Defense);
		}

	}

	weapon->RecastTime(MyGame::GameTime());
	
	if (special->GetIsSpecial()) {
		if (special->GetIsSpecialAttack()) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
		}
	}
}

void PlayerStateMove::Exit()
{

}

void PlayerStateMove::Enter()
{
	BaseWeapon* weapon = character_->GetWeapon();
	AnimationComponent* anima = character_->GetObject3D()->GetAnimationComponent();
	//weapon->GetTimer().t = 0.0f;
	weapon->GetObject3D()->SetIsDraw(false);
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
}

#pragma endregion 

#pragma region Jump

// 更新
void PlayerStateJump::Update() {
	AnimationComponent* anima = character_->GetObject3D()->GetAnimationComponent();
	Input* input = character_->GetInput();


}

// 終了
void PlayerStateJump::Exit() {

}
// 初期化
void PlayerStateJump::Enter() {


}

#pragma endregion // ジャンプ


#pragma region Attack

void PlayerStateAttack::Update()
{
	BaseWeapon* weapon = character_->GetWeapon();

	weapon->GetComboStateMachine()->Update(character_->GetTime());
}

void PlayerStateAttack::Exit()
{
	AnimationComponent* anima = character_->GetObject3D()->GetAnimationComponent();
	// 武器
	character_->GetWeapon()->GetComboStateMachine()->HandleInput(AttackInput::Light);
	character_->GetWeapon()->GetObject3D()->SetIsDraw(false);
	character_->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);

	// アニメーション
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
}

void PlayerStateAttack::Enter()
{
	BaseWeapon* weapon = character_->GetWeapon();

	// 武器
	weapon->GetComboStateMachine()->Update(character_->GetTime());
	weapon->GetObject3D()->SetIsDraw(true);
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
	weapon->GetColliderComponent()->contactRecord_.Clear();
}

#pragma endregion

#pragma region MyRegion



void PlayerStateSpecial::Update()
{
	BaseSpecial* special = character_->GetSpecial();
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	character_->Velocity() = {};
	int time = 0;
	player->GetPlayerUI()->SetIsTextRB(false);
	//ui_->SetIsTextRB(false);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special);
	rengeSp->InAction();
	rengeSp->SetIsDraw(false);
	if (special->GetPhese() == 0) {
		player->GetMoveComponent()->Move(*player->GetObject3D()->GetTransformComponent(), player->GetInput());
		player->GetPlayerUI()->SetIsTextRB(true);
		rengeSp->SetIsDraw(true);
	}
	if (special->GetPhese() == 2) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
	}
}

void PlayerStateSpecial::Exit()
{
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	player->GetPlayerUI()->SetIsTextRB(false);
}

void PlayerStateSpecial::Enter()
{
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	player->GetSpecial()->SetPhese(0);
	player->GetSpecial()->SetGauge(0);
	player->GetWeapon()->GetObject3D()->SetIsDraw(false);
}

#pragma endregion // 必殺技

#pragma region Skill
// 更新
void PlayerStateSkill::Update() {

	timer_ += character_->GetTime();
	if (changeTimer_ <= timer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
	}

};

// 終了
void PlayerStateSkill::Exit() {

};
// 初期化
void PlayerStateSkill::Enter() {
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	timer_ = 0.0f;
	player->GetBulletManager()->GenerateBullet(BulletManager::BulletType::kPlayerStan, player->GetWorldTransform().worldMat_.GetWorldPosition());
};
#pragma endregion // スキル

#pragma region Defense

// 更新
void PlayerStateDefense::Update() {
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
	}

};

// 終了
void PlayerStateDefense::Exit() {
	isDifense_ = false;
	character_->GetCombatStatComponent()->damageReduction_ = 0.0f;
	character_->GetCharacterParameterComponent().Stamina().rateFluctuation = 5.0f;
};
// 初期化
void PlayerStateDefense::Enter() {
	isDifense_ = true;
};

#pragma endregion // 防御

#pragma region Fainting

// 更新
void PlayerStateFainting::Update() {

	timer_ += character_->GetTime();

	if (timer_ <= faintingTimer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
	}

}

// 終了
void PlayerStateFainting::Exit() {
};
// 初期化
void PlayerStateFainting::Enter() {
};

#pragma endregion // 気絶



