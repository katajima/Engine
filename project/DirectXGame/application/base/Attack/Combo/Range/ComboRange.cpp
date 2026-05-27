#include "ComboRange.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Bullet/Base/BulletSpawn.h"
#include <algorithm>

// 開始
void Combo::ComboRange::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
	bulletSpawn = owner->GetBulletSpawn();
	bulletCount = 0;
	nextShotTime = data_.rangeWindowStart;
}
// 更新
void Combo::ComboRange::Update(const Character::CharacterContext& ctx, float timer) {
	if (!bulletSpawn || data_.count <= bulletCount ||
		timer < data_.rangeWindowStart || timer > data_.rangeWindowEnd) {
		return;
	}

	const float interval = (std::max)(data_.interval, 0.001f);
	while (bulletCount < data_.count &&
		timer >= nextShotTime && nextShotTime <= data_.rangeWindowEnd) {
		BulletInfo info{};
		info.position = ctx.position;
		info.targetPos = ctx.target ? ctx.target->GetWorldPosition() : ctx.position + ctx.direction;
		info.speed = data_.speed;
		info.damage = data_.damage;
		info.type = ProjectileType::NORMAL;
		bulletSpawn->GenerateBullet(BulletType::kPlayerBullet, info);

		bulletCount++;
		nextShotTime += interval;
	}
}
// 終了
void Combo::ComboRange::Exit(Character::BaseCharacter* owner) {
	bulletCount = 0;
	nextShotTime = 0.0f;
	bulletSpawn = nullptr;
}
