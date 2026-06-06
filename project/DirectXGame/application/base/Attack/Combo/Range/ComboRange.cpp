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
			info.targetPos = ctx.position + ResolveAimDirection(ctx);
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
			throwData.alignToDirection = data_.subWeaponAlignToDirection;
			throwData.useSpin = data_.subWeaponUseSpin;
			throwData.rotateOffset = data_.subWeaponRotateOffset;
			subWeapon->SetThrowData(throwData);

			// コンボの狙い方設定に従って投擲方向を決める
			Vector3 throwDirection = ResolveAimDirection(ctx);
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

Vector3 Combo::ComboRange::ResolveAimDirection(const Character::CharacterContext& ctx) const {
	// 基本方向はプレイヤーの向きにする
	Vector3 direction = ctx.direction;

	// カメラ方向指定ならカメラの前方向を使う
	if (data_.lockOnType == RangeLockOnType::kCamera) {
		direction = ctx.cameraDirection;
	}

	// ターゲット指定なら半径内にいる時だけターゲット方向へ補正する
	if (data_.lockOnType == RangeLockOnType::kTarget && ctx.target) {
		const float radius = (std::max)(data_.lockOnStartRadius, 0.0f);
		if (ctx.position.DistanceXZ(ctx.target->GetWorldPosition()) <= radius) {
			direction = ctx.target->GetWorldPosition() - ctx.position;
		}
	}

	// 水平方向の投擲に寄せ、ゼロ方向なら前方へフォールバックする
	direction.y = 0.0f;
	if (direction.LengthSq() <= 0.0001f) {
		direction = { 0.0f,0.0f,1.0f };
	}
	return direction.Normalize();
}
// 終了
void Combo::ComboRange::Exit(Character::BaseCharacter* owner) {
	owner_ = nullptr;
	bulletCount = 0;
	nextShotTime = 0.0f;
	bulletSpawn = nullptr;
}
