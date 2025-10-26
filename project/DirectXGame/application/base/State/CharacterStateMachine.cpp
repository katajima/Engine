#include "CharacterStateMachine.h"

void CharacterStateMachine::Init(BaseCharacter* character, CharacterMainState initialState)
{
	character_ = character;
	ChangeState(initialState);
}

void CharacterStateMachine::Update() {
	state_->Update();

	isOneFrameOneState_ = false;
}