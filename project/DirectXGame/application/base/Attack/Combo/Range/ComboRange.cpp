#include "ComboRange.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include "DirectXGame/application/base/Bullet/Base/BulletSpawn.h"
#include "DirectXGame/application/base/Weapon/Player/PlayerSubWeapon.h"
#include <algorithm>

// 開始
void Combo::ComboRange::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
	owner_ = owner;
	bulletSpawn = owner->GetBulletSpawn();
	bulletCount = 0;
	nextShotTime = data_.rangeWindowStart;
}
// 更新
void Combo::ComboRange::Update(const Character::CharacterContext& ctx, float timer) {
	if (data_.count <= bulletCount ||
		timer < data_.rangeWindowStart || timer > data_.rangeWindowEnd) {
		return;
	}

	const float interval = (std::max)(data_.interval, 0.001f);
	while (bulletCount < data_.count &&
		timer >= nextShotTime && nextShotTime <= data_.rangeWindowEnd) {
		bool didUseRange = false;	// 今回の発射タイミングで攻撃物を使用できたか
		// RangeType別に使用する攻撃物を切り替える
		switch (data_.rangeType) {
		case RangeType::kBullet:
		{
			if (!bulletSpawn) {
				break;
			}
			// 弾タイプは既存の弾生成処理を使用する
			BulletInfo info{};
			info.position = ctx.position;
			info.targetPos = ctx.target ? ctx.target->GetWorldPosition() : ctx.position + ctx.direction;
			info.speed = data_.speed;
			info.damage = data_.damage;
			info.type = ProjectileType::NORMAL;
			bulletSpawn->GenerateBullet(BulletType::kPlayerBullet, info);
			didUseRange = true;
			break;
		}
		case RangeType::kSubWeapon:
		{
			// サブ武器タイプはプレイヤーが持つPlayerSubWeaponを投擲する
			auto* player = dynamic_cast<Character::BasePlayer*>(owner_);
			if (!player) {
				break;
			}
			auto* subWeapon = dynamic_cast<PlayerSubWeapon*>(player->GetSubWeapon());
			if (!subWeapon || subWeapon->IsThrowing()) {
				break;
			}

			// コンボ保存項目の値をサブ武器の投擲データへ反映する
			PlayerSubWeaponThrowData throwData{};
			throwData.idleOffset = data_.subWeaponIdleOffset;
			throwData.throwSpeed = data_.subWeaponThrowSpeed;
			throwData.throwLifeTime = data_.subWeaponThrowLifeTime;
			throwData.returnTime = data_.subWeaponReturnTime;
			throwData.spinSpeed = data_.subWeaponSpinSpeed;
			subWeapon->SetThrowData(throwData);

			// ターゲットがいるならターゲット方向、いなければコンテキストの向きへ投げる
			Vector3 throwDirection = ctx.target ? ctx.target->GetWorldPosition() - ctx.position : ctx.direction;
			subWeapon->Throw(ctx.position + data_.subWeaponStartOffset, throwDirection);
			didUseRange = true;
			break;
		}
		case RangeType::kWeapon:
		default:
			// 通常武器投擲は今後の拡張枠として保存だけ行う
			break;
		}

		// 使用できた時だけ発射数を進める
		if (didUseRange) {
			bulletCount++;
		}
		nextShotTime += interval;
	}
}
// 終了
void Combo::ComboRange::Exit(Character::BaseCharacter* owner) {
	owner_ = nullptr;
	bulletCount = 0;
	nextShotTime = 0.0f;
	bulletSpawn = nullptr;
}
