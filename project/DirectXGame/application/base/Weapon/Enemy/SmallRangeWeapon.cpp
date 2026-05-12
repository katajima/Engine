#include "SmallRangeWeapon.h"
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include"DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include<DirectXGame/application/base/Bullet/Base/BulletManager.h>
#include"DirectXGame/application/base/Effect/Effect.h"
#include <DirectXGame/application/base/Bullet/Base/BulletSpawn.h>

void SmallRangeWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
	this->inputSystem = inputSystem;						// インプット
	this->entityManager = entityManager; // エンティティ3d
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->InitializeInstancing(entityManager, globalVariables, "enemyMuzzleSS01", "enemyMuzzleSS01.obj", "",
		false, false, this, Engine::ObjectInstans::TransparencyType::kNo);
	objectComponent_->SetInstancingSRT({ 1.0f,1.0f,1.0f }, {Math::DegreesToRadians(10),0,0}, position);	// SRT設定
	objectComponent_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化
}

void SmallRangeWeapon::Update()
{
	if (target) {
		targetPos_ = target->GetWorldPosition();
	}
	
	objectComponent_->Update();
}

void SmallRangeWeapon::DrawEffect()
{
}

void SmallRangeWeapon::Draw2D()
{
}

void SmallRangeWeapon::Shot()
{
	// 弾の情報
	BulletInfo info{};
	info.damage = 10;						// ダメージ
	info.speed = rengedData_.bulletSpeed;	// スピード
	info.targetPos = targetPos_;			// ターゲット位置
	info.position = objectComponent_->GetWorldTransform().GetWorldPosition(); // 開始位置

	// モードによって弾の状態変更
	if (modeType_ == ModeType::Normal) {
		info.type = ProjectileType::NORMAL;
	}
	
	// 弾生成
	character->GetBulletSpawn()->GenerateBullet(BulletType::kEnemyBullet, info);
}
