#include "BasePlayer.h"

#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/FollowCamera/FollowCamera.h"

void BasePlayer::OnCollision(Collider* other)
{
	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		if (isAlive_) {
			if (!isInvincible_) {
				BaseEnemy* enemy = static_cast<BaseEnemy*>(other);
				uint32_t serialNumber = enemy->GetSerialNumber();

				// 接触履歴があれば何もせず抜ける
				if (contactRecord_.CheckHistory(serialNumber)) {
					return;
				}

				contactRecord_.AddHistory(serialNumber);


				followCamera_->GetViewProjection().SetShake(0.1f, { 1.5f,1.5f,1.5f });

			}
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemyWeapon)) {
	}
}

Vector3 BasePlayer::GetCenterPosition() const
{
	return object_->GetWorldPosition();
}
