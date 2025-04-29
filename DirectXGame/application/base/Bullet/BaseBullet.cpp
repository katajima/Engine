#include "BaseBullet.h"

#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"

uint32_t BaseBullet::nextSerialNumber = 0;

BaseBullet::BaseBullet()
{
	// シリアル番号を振る
	serialNumber = nextSerialNumber;
	// 次の番号を1加算
	++nextSerialNumber;

	parameter_ = {};
}

float BaseBullet::GetTimer() const
{
	return  MyGame::GameTime();
}

void BaseBullet::SetPlayer(Player* player)
{
	player_ = player;
}

void BaseBullet::SetEnemy(BaseEnemy* enemy)
{
	enemy_ = enemy;
}

void BaseBullet::SetTargetType(CollisionTypeIdDef type)
{
	switch (type)
	{
	case CollisionTypeIdDef::kdefault:
		isCollisioType_.isDefault = true;
		break;
	case CollisionTypeIdDef::kPlayer:
		isCollisioType_.isPlayer = true;
		break;
	case CollisionTypeIdDef::kPlayerWeapon:
		isCollisioType_.isPlayerWeapon = true;
		break;
	case CollisionTypeIdDef::kEnemy:
		isCollisioType_.isEnemy = true;
		break;
	case CollisionTypeIdDef::kEnemyWeapon:
		isCollisioType_.isEnemyWeapon = true;
		break;
	case CollisionTypeIdDef::kEnemyWeapon2:
		isCollisioType_.isEnemyWeapon2 = true;
		break;
	default:
		break;
	}

}

void BaseBullet::OnCollision(Collider* other)
{
	// 当たり判定を行わなければ抜ける
	if (!isCollision) {
		return;
	}


	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();




	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		BaseEnemy* enemy = static_cast<BaseEnemy*>(other);
		uint32_t serialNumber = enemy->GetSerialNumber();

		if (!isCollisioType_.isEnemy) {
			return;
		}

		// 接触履歴があれば何もせず抜ける
		if (contactRecord_.CheckHistory(serialNumber)) {
			return;
		}


		contactRecord_.AddHistory(serialNumber);

		if (enemy->GetAlive()) {

			enemy->AddDamage(parameter_.damege);

			enemy->SetHit();

			enemy->Emit();

			// 敵
			EnemyToColl();
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		Player* player = static_cast<Player*>(other);
		uint32_t serialNumber = player->GetSerialNumber();

		if (!isCollisioType_.isPlayer) {
			return;
		}

		// 接触履歴があれば何もせず抜ける
		if (contactRecord_.CheckHistory(serialNumber)) {
			return;
		}


		contactRecord_.AddHistory(serialNumber);

		if (player->GetAlive()) {

			player->AddDamege(parameter_.damege);

			// 敵
			PlayerToColl();
		}
	}

}

Vector3 BaseBullet::GetCenterPosition() const
{
	return object_->GetWorldPosition();
}
