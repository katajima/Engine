#include "BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"
#include "DirectXGame/application/base/FollowCamera/FollowCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"


uint32_t BaseEnemy::nextSerialNumber = 0;

void BaseEnemy::OnCollision(Collider* other)
{
	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		Player* player = static_cast<Player*>(other);
		uint32_t serialNumber = player->GetSerialNumber();

		if (isAlive_) {
			if (!player->GetInvincible()) {

				// 接触履歴があれば何もせず抜ける
				if (contactRecord_.CheckHistory(serialNumber)) {
					return;
				}

				contactRecord_.AddHistory(serialNumber);

				//followCamera_->GetViewProjection().SetShake(0.1f, { 1.5f,1.5f,1.5f });

				player->AddDamege(parameter_.damege);
			}
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {

	}
}

Vector3 BaseEnemy::GetCenterPosition() const
{
	return object_->GetWorldPosition();
}

BaseEnemy::BaseEnemy()
{
	// シリアル番号を振る
	serialNumber = nextSerialNumber;
	// 次の番号を1加算
	++nextSerialNumber;

	parameter_ = {};
}

void BaseEnemy::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
}
