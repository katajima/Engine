#include "CharacterStateMachine.h"


namespace Character {
	void CharacterStateMachine::Init(BaseCharacter* character, CharacterMainState initialState)
	{
		this->character = character;
		ChangeState(initialState);
	}

	void CharacterStateMachine::Update(const CharacterContext& ctx) {
		// 更新
		state_->Update(ctx);
		isOneFrameOneState_ = false;
	}
}