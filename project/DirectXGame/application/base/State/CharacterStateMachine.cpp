#include "CharacterStateMachine.h"


namespace Character {
	void CharacterStateMachine::Init(BaseCharacter* character, CharacterMainState initialState)
	{
		character_ = character;
		ChangeState(initialState);
	}

	void CharacterStateMachine::Update() {
		// 更新
		state_->Update();
		isOneFrameOneState_ = false;
	}
}