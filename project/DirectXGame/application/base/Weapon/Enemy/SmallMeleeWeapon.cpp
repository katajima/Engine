#include "SmallMeleeWeapon.h"

void SmallMeleeWeapon::Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	input_ = input;						// インプット
	entity3DManager_ = entity3DManager; // エンティティ3d
	entity2DManager_ = entity2DManager; // エンティティ2d

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->InitializeInstancing(entity3DManager_, globalVariables_, "enemyWeaponSG01", "enemyWeaponSG01.obj", "", false, false, this);
	objectComponent_->SetInstancingSRT({4,4,4}, {}, position);	// SRT設定
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
