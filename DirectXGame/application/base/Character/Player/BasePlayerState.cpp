#include "BasePlayerState.h"
#include"Player.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

PlayerStateMove::PlayerStateMove(BasePlayer* player)
	: BasePlayerState("Move", player) {
}

void PlayerStateMove::Update()
{
	Input* input = player_->GetInput();
	BaseWeapon* weapon = player_->GetWeapon();
	BaseSpecial* special = player_->GetSpecial();

	if (input->IsControllerConnected()) {

		if (player_->GetSpecial()->GetIsSpecial()) {
			special->SetIsSpecialAttack(input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB));
		}

		weapon->GetAttackKeyFlag().IsNormalAttack = input->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
		if (weapon->GetAttackKeyFlag().IsNormalAttack) {
			weapon->SetIsAttack(true);
		}
	}

	// ジャンプ
	player_->Jump();

	// 移動
	player_->Move();

	weapon->RecastTime(MyGame::GameTime());
	if (weapon->IsAttack()) {
		if (weapon->GetIsRecastTimeOver()) {
			player_->ChangeState("Attack");
			weapon->GetTimer().t = 0.0f;
			weapon->TrueState();
		}
	}
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
	weapon->GetTimer().t = 0.0f;
	weapon->GetObject3D()->SetIsDraw(false);
	player_->Situations().isInvincible = false;
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);
}

PlayerStateAttack::PlayerStateAttack(BasePlayer* player)
	: BasePlayerState("Attack", player) {}

void PlayerStateAttack::Update()
{
	BaseWeapon* weapon = player_->GetWeapon();
	
	
	// 攻撃処理
	weapon->AttackUpdate();

	if (!weapon->GetisTrueState()) {
		player_->ChangeState("Move");
		return;
	}
}

void PlayerStateAttack::Exit()
{
	player_->GetWeapon()->GetObject3D()->SetIsDraw(false);
	player_->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);
}

void PlayerStateAttack::Enter()
{
	BaseWeapon* weapon = player_->GetWeapon();
	weapon->GetAttackKeyFlag().IsNormalAttack = true;
	weapon->KeyAttackTypes(player_->GetSituation().isJumping);
	weapon->AttackTypeInit(0);
	weapon->GetObject3D()->SetIsDraw(true);
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
	player_->Situations().isInvincible = true;
}

PlayerStateSpecial::PlayerStateSpecial(BasePlayer* player)
	: BasePlayerState("Special", player) {}

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
		player_->Move();
		player_->GetPlayerUI()->SetIsTextRB(true);
		rengeSp->SetIsDraw(true);
	}
	if (special->GetPhese() == 2) {
		player_->ChangeState("Move");
		//basicbehaviorRequest_ = BasicBehavior::kRoot;
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