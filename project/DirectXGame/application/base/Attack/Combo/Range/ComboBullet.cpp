#include "ComboBullet.h"
#include "DirectXGame/application/base/Bullet/Base/BulletSpawn.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"
#include <algorithm>

namespace Combo {

	void ComboBullet::Enter(Character::BaseCharacter* owner, const GlobalRange& data) {
		owner_ = owner;									// コンボ使用者
		bulletSpawn_ = owner_ ? owner_->GetBulletSpawn() : nullptr;	// 弾生成クラス
		bulletCount_ = 0;								// 発射数リセット
		nextShotTime_ = data.rangeWindowStart;			// 次の発射時間
	}

	void ComboBullet::Update(const Character::CharacterContext& ctx, float timer, const GlobalRange& data, const Vector3& aimTarget) {
		// 弾生成クラスが無い、または発射受付時間外なら何もしない
		if (!bulletSpawn_ || bulletCount_ >= data.count || timer < data.rangeWindowStart || timer > data.rangeWindowEnd) {
			return;
		}

		// 発射間隔に従って同一フレーム内の連射も処理する
		const float interval = (std::max)(data.interval, 0.001f);
		while (bulletCount_ < data.count && timer >= nextShotTime_ && nextShotTime_ <= data.rangeWindowEnd) {
			BulletInfo info{};					// 弾生成情報
			info.position = ctx.position;		// 発射位置
			info.targetPos = aimTarget;			// 狙う位置
			info.speed = data.speed;				// 弾速
			info.damage = data.damage;			// ダメージ
			info.type = ProjectileType::NORMAL;	// 通常弾
			bulletSpawn_->GenerateBullet(BulletType::kPlayerBullet, info);

			bulletCount_++;				// 発射済み数
			nextShotTime_ += interval;	// 次の発射時間
		}
	}

	void ComboBullet::Exit() {
		owner_ = nullptr;			// コンボ使用者を解除
		bulletSpawn_ = nullptr;		// 弾生成クラスを解除
		bulletCount_ = 0;			// 発射数リセット
		nextShotTime_ = 0.0f;		// 次発射時間リセット
	}
}
