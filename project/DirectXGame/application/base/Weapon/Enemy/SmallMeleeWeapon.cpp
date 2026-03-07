#include "SmallMeleeWeapon.h"

void SmallMeleeWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	this->inputSystem = inputSystem;			// インプット
	this->entityManager = entityManager;	// エンティティ3d
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->InitializeInstancing(entityManager, globalVariables, "enemyWeaponSG01", "enemyWeaponSG01.obj", "", 
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
