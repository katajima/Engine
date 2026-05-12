#include "BasePlayer.h"
#include <DirectXGame/application/base/Special/Base/BaseSpecial.h>
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"

Character::BasePlayer::~BasePlayer() = default;

// SPゲージ加算
void Character::BasePlayer::AddSpGauge(int d) { special_->AddGauge(d); };
// SP発動可能？
bool Character::BasePlayer::GetIsSpecial() const { return special_->GetIsSpecial(); }
