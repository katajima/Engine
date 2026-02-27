#include "PlayerCarStateMachine.h"

void PlayerCarStateMachine::Initialize(PlayerCar* car, CarMainState initialState) {
	// 所有者設定
	playerCar = car;
	// ステート変更
	ChangeState(initialState);
}

void PlayerCarStateMachine::Update(float dt) {
	// 更新
	state_->Update(dt);
};
