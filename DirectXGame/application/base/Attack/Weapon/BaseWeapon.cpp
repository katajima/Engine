#include "BaseWeapon.h"
#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Base/BasePlayer.h"


void BaseWeapon::OnCollision(Collider* other)
{
	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		BaseEnemy* enemy = static_cast<BaseEnemy*>(other);
		uint32_t serialNumber = enemy->GetSerialNumber();



		// 接触履歴があれば何もせず抜ける
		if (contactRecord_.CheckHistory(serialNumber)) {
			return;
		}

		UsePlayer();
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		BasePlayer* player = static_cast<BasePlayer*>(other);
		uint32_t serialNumber = player->GetSerialNumber();

		// 接触履歴があれば何もせず抜ける
		if (contactRecord_.CheckHistory(serialNumber)) {
			return;
		}

		UseEnemy();

	}
}

Vector3 BaseWeapon::GetCenterPosition() const
{
	const Vector3 ofset = offset;
	// ワールド座標に変換
	Vector3 worldPos = Transforms(ofset, object_->worldtransform_.worldMat_);

	return worldPos;
}

void BaseWeapon::ContactRecordClear()
{
	contactRecord_.Clear();
}

