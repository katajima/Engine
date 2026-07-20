#include "BasePlayer.h"
#include <DirectXGame/application/base/Special/Base/BaseSpecial.h>
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"

Character::BasePlayer::BasePlayer(){
	
}

Character::BasePlayer::~BasePlayer() = default;

// SPゲージ加算
void Character::BasePlayer::AddSpGauge(int d) { special_->AddGauge(d); };
// SP発動可能？
bool Character::BasePlayer::GetIsSpecial() const { return special_->GetIsSpecial(); }
// 被弾後無敵の残り時間を更新
void Character::BasePlayer::UpdateDamageInvincible(float dt) {
	// タイマーが残っていない場合は処理しない。
	if (damageInvincibleTimer_ <= 0.0f) {
		return;
	}

	damageInvincibleTimer_ -= dt;
	if (damageInvincibleTimer_ < 0.0f) {
		damageInvincibleTimer_ = 0.0f;
	}
}

// 被弾後無敵中か取得
bool Character::BasePlayer::IsDamageInvincible() const {
	return damageInvincibleTimer_ > 0.0f;
}

// ダメージ無敵中ならHP減算を無視
bool Character::BasePlayer::ShouldIgnoreDamage(float damage) const {
	// 回復や0ダメージは無敵で止めず、正のダメージだけを無視対象にする。
	return damage > 0.0f && IsDamageInvincible();
}

// ダメージが通った時に無敵時間を開始
void Character::BasePlayer::OnDamageApplied(float damage) {
	// 正のダメージだけを被弾として扱い、被弾直後の連続ダメージを防ぐ。
	if (damage > 0.0f) {
		damageInvincibleTimer_ = damageInvincibleDuration_;
	}
}
