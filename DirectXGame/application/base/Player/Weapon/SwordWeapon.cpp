#include "SwordWeapon.h"

void SwordWeapon::Initialize(Entity3DManager* entity3DManager, Camera* camera)
{
	//Collider::Initialize(camera);
	//Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));


	object_ = std::make_unique<Object3d>();
	object_->Initialize(entity3DManager);
	object_->SetCamera(camera);
	object_->SetModel("Sword.obj");
}

void SwordWeapon::Update()
{
	object_->Update();
}

void SwordWeapon::Draw()
{
	object_->Draw();
}

void SwordWeapon::UsePlayer()
{
}

void SwordWeapon::UseEnemy()
{
}
