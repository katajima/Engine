#include "SmallMeleeWeapon.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"

void SmallMeleeWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	(void)camera;
	// 敵近接武器が参照する入力とエンティティ管理を保持する
	this->inputSystem = inputSystem;			// インプット
	this->entityManager = entityManager;	// エンティティ3d
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->InitializeInstancing(entityManager, globalVariables, "enemyWeaponSG01", "enemyWeaponSG01.obj", "", 
		false, false, this, Engine::ObjectInstans::TransparencyType::kNo);
	objectComponent_->SetInstancingSRT({1,1,1}, {}, position);	// SRT設定
	objectComponent_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化

}

void SmallMeleeWeapon::Update()
{
	// 武器モデルのTransformや当たり判定用コンポーネントを更新する
	objectComponent_->Update();
}

void SmallMeleeWeapon::DrawEffect()
{
	// 近接武器専用エフェクトは未実装
}

void SmallMeleeWeapon::Draw2D()
{
	// 近接武器専用2D表示は未実装
}
