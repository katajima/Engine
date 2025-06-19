#include "playerWeapon.h"
#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"
#include "DirectXGame/application/base/Player/Base/BasePlayer.h"

void playerWeapon::Initialize(Entity3DManager* entity3DManager, Camera* camera)
{
	//Collider::Initialize(camera);
	//Collider::SetColliderType(static_cast<uint32_t>(ColliderType::Sphere));
	//Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));



	objectWeapon_.Initialize(entity3DManager);
	objectWeapon_.SetCamera(camera);
	objectWeapon_.SetIsDraw(false);
	objectWeapon_.SetModel("Sword.obj");

}

void playerWeapon::Update()
{
	objectWeapon_.Update();
}

void playerWeapon::Draw()
{
	objectWeapon_.Draw();
}



void playerWeapon::ContactRecordClear()
{
	contactRecord_.Clear();
}

void playerWeapon::SetPlayer(Player* player)
{
	player_ = player;
}

void playerWeapon::SetPlayer(BasePlayer* player)
{
	basePlayer_ = player;
}
