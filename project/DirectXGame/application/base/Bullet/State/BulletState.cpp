#include "BulletState.h"

void BulletStateMachine::Initialize(BaseBullet* object, BulletState initialState){
	object_ = object;
	ChangeState(initialState);
}

void BulletStateMachine::Update(float dt) {
	state_->Update(dt);
}
