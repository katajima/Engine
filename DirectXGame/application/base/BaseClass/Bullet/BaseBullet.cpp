#include "BaseBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"


BaseBullet::BaseBullet()
{
	parameter_ = {};
}

BaseBullet::~BaseBullet()
{
	
	object_->IsDelete();
}

float BaseBullet::GetTimer() const
{
	return  MyGame::GameTime();
}

void BaseBullet::SetPlayer(BasePlayer* player)
{
	player_ = player;
}

void BaseBullet::SetEnemy(BaseEnemy* enemy)
{
	enemy_ = enemy;
}

//void BaseBullet::SetTargetType(CollisionTypeIdDef type)
//{
//	switch (type)
//	{
//	case CollisionTypeIdDef::kdefault:
//		isCollisioType_.isDefault = true;
//		break;
//	case CollisionTypeIdDef::kPlayer:
//		isCollisioType_.isPlayer = true;
//		break;
//	case CollisionTypeIdDef::kPlayerWeapon:
//		isCollisioType_.isPlayerWeapon = true;
//		break;
//	case CollisionTypeIdDef::kEnemy:
//		isCollisioType_.isEnemy = true;
//		break;
//	case CollisionTypeIdDef::kEnemyWeapon:
//		isCollisioType_.isEnemyWeapon = true;
//		break;
//	case CollisionTypeIdDef::kEnemyWeapon2:
//		isCollisioType_.isEnemyWeapon2 = true;
//		break;
//	default:
//		break;
//	}
//
//}
