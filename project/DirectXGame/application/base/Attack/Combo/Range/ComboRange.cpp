#include "ComboRange.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

// 開始
void Combo::ComboRange::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
	bulletSpawn = owner->GetBulletSpawn();
	bulletCount = 0;
}
// 更新
void Combo::ComboRange::Update(const Character::CharacterContext& ctx, float timer) {

	if (data_.count < bulletCount) {
		bulletCount++;
	}


}
// 終了
void Combo::ComboRange::Exit(Character::BaseCharacter* owner) {
	bulletCount = 0;
}