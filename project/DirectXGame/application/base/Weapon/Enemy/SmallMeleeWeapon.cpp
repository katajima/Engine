#include "SmallMeleeWeapon.h"

void SmallMeleeWeapon::Initialize(InputSystem* inputSystem, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	this->inputSystem = inputSystem;			// インプット
	this->entity3DManager = entity3DManager;	// エンティティ3d
	this->entity2DManager = entity2DManager;	// エンティティ2d

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->InitializeInstancing(entity3DManager, globalVariables, "enemyWeaponSG01", "enemyWeaponSG01.obj", "", 
		false, false, this, Engine::Object3dInstansManager::TransparencyType::kNo);
	objectComponent_->SetInstancingSRT({1,1,1}, {}, position);	// SRT設定
	objectComponent_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化

}

void SmallMeleeWeapon::Update()
{
	objectComponent_->Update();
}

void SmallMeleeWeapon::DrawEffect()
{
}

void SmallMeleeWeapon::Draw2D()
{
}
