#include "DummyEnemyState.h"
#include "DummyEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
void Character::DummyEnemyDamageState::Update(const CharacterContext& ctx) {
	if (character->GetHP() <= 0) {
		//character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
	}
	if (character->GetHitMotionSystem()->IsFinished()) {
		character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
	}
}
