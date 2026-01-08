#include "SpecialPointStateMachine.h"


void SpecialPointStateMachine::Initialize(SpecalPoint* object, SpecialPointState initialState){
	object_ = object;
	ChangeState(initialState);
}

void SpecialPointStateMachine::Update(float dt) {
	// 更新
	state_->Update(dt);
}