#include "PlayerCarState.h"
#include "PlayerCar.h"
#include "DirectXGame/engine/Math/Random.h"

namespace {
	// タイトル画面で車が走り出す初速度。
	const Vector3 kTitleMoveInitialVelocity = { 4.0f, 0.0f, 0.0f };
	// タイトル画面で車へ毎フレーム加える加速度。
	constexpr float kTitleMoveAccelerationPerFrame = 0.5f;
	// 発進準備中にライトを点灯するまでの時間。
	constexpr float kHeadlightTurnOnDelaySeconds = 0.2f;
	// 発進準備中に車体へ加える上下振動の範囲。
	constexpr float kPreparationShakeMinOffset = -0.03f;
	constexpr float kPreparationShakeMaxOffset = 0.03f;
	// セレクト画面でタイヤから放出する埃の方向と範囲。
	const Vector3 kSelectDustDirection = { 0.0f, 0.0f, 15.0f };
	const Vector3 kSelectDustRange = { 0.0f, 0.0f, 5.0f };
	// リザルト画面でスクラップを放出する方向と範囲。
	const Vector3 kScrapEmitDirection = { 0.0f, 15.0f, 0.0f };
	const Vector3 kScrapEmitRange = { 0.0f, 5.0f, 0.0f };
}

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
	velocity_ = kTitleMoveInitialVelocity;
}

void CarTitleMoveState::Update(float dt)
{
	velocity_.x += kTitleMoveAccelerationPerFrame;
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

	if (timer_ >= kHeadlightTurnOnDelaySeconds) {
		GetPlayerCar()->GetLFSpotLight()->spot.isLight = true;
		GetPlayerCar()->GetRFSpotLight()->spot.isLight = true;
	}

	GetPlayerCar()->GetBodyWorldTransform().translate_.y = pos_.y +
		Random::RandomFloat(kPreparationShakeMinOffset, kPreparationShakeMaxOffset);
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
	GetPlayerCar()->Emit(GetPlayerCar()->GetLFTierWorldPosition() + dustOffset_, kSelectDustDirection, kSelectDustRange);
	GetPlayerCar()->Emit(GetPlayerCar()->GetLBTierWorldPosition() + dustOffset_, kSelectDustDirection, kSelectDustRange);
	GetPlayerCar()->Emit(GetPlayerCar()->GetRFTierWorldPosition() + dustOffset_, kSelectDustDirection, kSelectDustRange);
	GetPlayerCar()->Emit(GetPlayerCar()->GetRBTierWorldPosition() + dustOffset_, kSelectDustDirection, kSelectDustRange);
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
		GetPlayerCar()->EmitScrapBox(GetPlayerCar()->GetScrapBoxWorldPosition() + scrapOffset_,
			kScrapEmitDirection, kScrapEmitRange);
	}
}

void ResultCashExchangeState::Exit()
{
}
