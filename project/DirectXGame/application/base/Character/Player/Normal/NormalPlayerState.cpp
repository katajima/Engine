#include "NormalPlayerState.h"
#include"Player.h"
#include "DirectXGame/engine/MyGame/MyGame.h"




PlayerStateMove::PlayerStateMove(BasePlayer* player)
	: BasePlayerState("Move", player) {
}

void PlayerStateMove::Update()
{
	AnimationComponent* anima = player_->GetObject3D()->GetAnimationComponent();
	Input* input = player_->GetInput();
	BaseWeapon* weapon = player_->GetWeapon();
	BaseSpecial* special = player_->GetSpecial();

	weapon->GetObject3D()->SetIsDraw(true);

	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);

	if(player_->GetCharacterStateComponent().IsJumping()){
		if (player_->GetObject3D()->GetRigidBodyComponent()->Velocity().y <=  0.0f) {
			anima->SetAnimetion("Fall", 0.01f);
		}
		else {
			anima->SetAnimetion("Fall", 0.01f);
		}
	}
	else {
		if (player_->GetVelocity().Length() != 0) {

			anima->SetAnimetion("Walk", 0.1f);
		}
		else {
			anima->SetAnimetion("Idle1", 0.1f);
		}
	}
	

	if (input->IsControllerConnected()) {

		if (player_->GetSpecial()->GetIsSpecial()) {
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
			player_->ChangeState("Special");
		}
	}
}



void PlayerStateMove::Exit()
{

}

void PlayerStateMove::Enter()
{
	BaseWeapon* weapon = player_->GetWeapon();
	AnimationComponent* anima = player_->GetObject3D()->GetAnimationComponent();
	//weapon->GetTimer().t = 0.0f;
	weapon->GetObject3D()->SetIsDraw(false);
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
}

PlayerStateAttack::PlayerStateAttack(BasePlayer* player)
	: BasePlayerState("Attack", player) {
}

void PlayerStateAttack::Update()
{
	BaseWeapon* weapon = player_->GetWeapon();
	
	

	//// コンボの終了判定：次がない && アニメ終了
	//if (player_->GetWeapon()->GetComboStateMachine()->IsComboFinished() && player_->GetObject3D()->IsAnimationFinished()) {
	//	player_->ChangeState("Move");
	//}

	weapon->GetComboStateMachine()->Update(player_->GetTime());

	// 攻撃処理
	//weapon->AttackUpdate();

	///if (!weapon->GetAttackInput().GetIsState()) {
	//	player_->ChangeState("Move");
	//	return;
	//}
}


void PlayerStateAttack::Exit()
{
	AnimationComponent* anima = player_->GetObject3D()->GetAnimationComponent();
	// 武器
	player_->GetWeapon()->GetComboStateMachine()->HandleInput(AttackInput::Light);
	player_->GetWeapon()->GetObject3D()->SetIsDraw(false);
	player_->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);
	
	// アニメーション
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
}

void PlayerStateAttack::Enter()
{
	BaseWeapon* weapon = player_->GetWeapon();
	
	// 武器
	weapon->GetComboStateMachine()->Update(player_->GetTime());
	weapon->GetObject3D()->SetIsDraw(true);
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
	weapon->GetColliderComponent()->contactRecord_.Clear();
}


#pragma region MyRegion

PlayerStateSpecial::PlayerStateSpecial(BasePlayer* player)
	: BasePlayerState("Special", player) {
}

void PlayerStateSpecial::Update()
{
	BaseSpecial* special = player_->GetSpecial();

	player_->Velocity() = {};
	int time = 0;
	player_->GetPlayerUI()->SetIsTextRB(false);
	//ui_->SetIsTextRB(false);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special);
	rengeSp->InAction();
	rengeSp->SetIsDraw(false);
	if (special->GetPhese() == 0) {
		player_->GetMoveComponent()->Move(*player_->GetObject3D()->GetTransformComponent(), player_->GetInput());
		player_->GetPlayerUI()->SetIsTextRB(true);
		rengeSp->SetIsDraw(true);
	}
	if (special->GetPhese() == 2) {
		player_->ChangeState("Move");
	}
}

void PlayerStateSpecial::Exit()
{
	player_->GetPlayerUI()->SetIsTextRB(false);
}

void PlayerStateSpecial::Enter()
{
	player_->GetSpecial()->SetPhese(0);
	player_->GetSpecial()->SetGauge(0);
	player_->GetWeapon()->GetObject3D()->SetIsDraw(false);
}

#pragma endregion // 必殺技

