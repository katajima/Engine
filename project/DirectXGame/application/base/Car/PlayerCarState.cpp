#include "PlayerCarState.h"
#include "PlayerCar.h"
#include "DirectXGame/engine/Math/Random.h"

#pragma region Idle

void CarIdleState::Enter()
{
}

void CarIdleState::Update(float dt)
{
}

void CarIdleState::Exit()
{
}

#pragma endregion

#pragma region Move

void CarTitleMoveState::Enter()
{
	// 速度
	velocity_ = { 4.0f,0.0f,0.0f };
}

void CarTitleMoveState::Update(float dt)
{
	velocity_.x += 0.5f;
	// 移動
	GetPlayerCar()->GetBodyWorldTransform().translate_ += velocity_ * dt;
	GetPlayerCar()->GetBodyWorldTransform().translate_.y = pos_.y;

	// タイヤ回転
	GetPlayerCar()->GetLFTierWorldTransform().rotate_.x += velocity_.x * dt;
	GetPlayerCar()->GetLBTierWorldTransform().rotate_.x += velocity_.x * dt;
	GetPlayerCar()->GetRFTierWorldTransform().rotate_.x += velocity_.x * dt;
	GetPlayerCar()->GetRBTierWorldTransform().rotate_.x += velocity_.x * dt;

	// 埃出現
	GetPlayerCar()->Emit(GetPlayerCar()->GetLFTierWorldPosition() + dustOffset_);
	GetPlayerCar()->Emit(GetPlayerCar()->GetLBTierWorldPosition() + dustOffset_);
	GetPlayerCar()->Emit(GetPlayerCar()->GetRFTierWorldPosition() + dustOffset_);
	GetPlayerCar()->Emit(GetPlayerCar()->GetRBTierWorldPosition() + dustOffset_);
}

void CarTitleMoveState::Exit()
{
}

#pragma endregion

#pragma region PreparationMove


void CarPreparationMoveState::Enter()
{
}

void CarPreparationMoveState::Update(float dt) {
	timer_ += dt;

	if (timer_ >= 0.2f) {
		GetPlayerCar()->GetLFSpotLight()->spot.isLight = true;
		GetPlayerCar()->GetRFSpotLight()->spot.isLight = true;
	}

	GetPlayerCar()->GetBodyWorldTransform().translate_.y = pos_.y + Random::RandomFloat(-0.03f, 0.03f);
}

void CarPreparationMoveState::Exit()
{
}


#pragma endregion

#pragma region SelectMove


void CarSelectMove::Enter(){
}

void CarSelectMove::Update(float dt){
	// タイヤ回転
	GetPlayerCar()->GetLFTierWorldTransform().rotate_.x += velocity_.x * dt;
	GetPlayerCar()->GetLBTierWorldTransform().rotate_.x += velocity_.x * dt;
	GetPlayerCar()->GetRFTierWorldTransform().rotate_.x += velocity_.x * dt;
	GetPlayerCar()->GetRBTierWorldTransform().rotate_.x += velocity_.x * dt;

	// 埃出現
	GetPlayerCar()->Emit(GetPlayerCar()->GetLFTierWorldPosition() + dustOffset_, {0,0,15},{0,0,5});
	GetPlayerCar()->Emit(GetPlayerCar()->GetLBTierWorldPosition() + dustOffset_, {0,0,15},{0,0,5});
	GetPlayerCar()->Emit(GetPlayerCar()->GetRFTierWorldPosition() + dustOffset_, {0,0,15},{0,0,5});
	GetPlayerCar()->Emit(GetPlayerCar()->GetRBTierWorldPosition() + dustOffset_, {0,0,15},{0,0,5});
}

void CarSelectMove::Exit(){
}

#pragma endregion

void ResultCashExchangeState::Enter()
{
	GetPlayerCar()->GetBodyWorldTransform().translate_ = pos_;
}

void ResultCashExchangeState::Update(float dt)
{
	scrapTimer_ += dt;
	if(scrapTimer_ >= scrapInterval_) {
		scrapTimer_ = 0.0f;
		GetPlayerCar()->EmitScrapBox(GetPlayerCar()->GetScrapBoxWorldPosition() + scrapOffset_, { 0,15, 0}, { 0,5,0 });
	}
}

void ResultCashExchangeState::Exit()
{
}
