#include "DummyEnemyState.h"
#include "DummyEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"


void Character::DummyEnemyDamageState::Update(const CharacterContext& ctx) {
	if (character->GetHitMotionSystem()->IsFinished()) {
		character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
	}
}
