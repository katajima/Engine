#include "NormalPlayerState.h"
#include"Player.h"
#include "DirectXGame/engine/MyGame/MyGame.h"






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

	if(character_->GetCharacterStateComponent().IsJumping()){
		if (character_->GetObject3D()->GetRigidBodyComponent()->Velocity().y <=  0.0f) {
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



		//weapon->GetAttackInput().GetAttackKeyFlag().IsNormalAttack = input->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
		//if (weapon->GetAttackInput().GetAttackKeyFlag().IsNormalAttack) {
		//	weapon->GetAttackInput().SetIsAttack(true);
		//}
	}

	weapon->RecastTime(MyGame::GameTime());
	//if (weapon->GetAttackInput().GetIsAttack()) {
		//if (weapon->GetIsRecastTimeOver()) {
		//	player_->ChangeState("Attack");
		//	weapon->GetTimer().t = 0.0f;
		//	weapon->GetAttackInput().TrueState();
		//}
	//}
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



void PlayerStateAttack::Update()
{
	BaseWeapon* weapon = character_->GetWeapon();
	
	

	//// コンボの終了判定：次がない && アニメ終了
	//if (player_->GetWeapon()->GetComboStateMachine()->IsComboFinished() && player_->GetObject3D()->IsAnimationFinished()) {
	//	player_->ChangeState("Move");
	//}

	weapon->GetComboStateMachine()->Update(character_->GetTime());

	// 攻撃処理
	//weapon->AttackUpdate();

	///if (!weapon->GetAttackInput().GetIsState()) {
	//	player_->ChangeState("Move");
	//	return;
	//}
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

