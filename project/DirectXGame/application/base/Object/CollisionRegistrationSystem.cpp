#include "CollisionRegistrationSystem.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>

void CollisionRegistrationSystem::Initialize(Engine::GlobalVariables* globalVariables,Engine::LineCommon* lineCommon, HitBox::System* hitBoxSystem,
	Character::CharacterManager* characterManager,SpecalPointManager* specalPointManager, BulletManager* bulletManager){

	this->lineCommon = lineCommon;
	this->hitBoxSystem = hitBoxSystem;	
	this->characterManager = characterManager;	
	this->specalPointManager = specalPointManager;	
	this->bulletManager = bulletManager;	


	// 衝突マネージャの生成
	Vector3 sizeAABB = { 300,25,300 };
	collisionManager_ = std::make_unique<Engine::CollisionManager>();
	collisionManager_->Initialize(globalVariables, AABB(-sizeAABB, sizeAABB));
}

void CollisionRegistrationSystem::RegisterAllCollisions(){
	// キャラクターセット
	for (auto caracter : characterManager->GetCharacters()) {
		if (caracter->GetColliderComponent()) {
			if (caracter->GetAlive() && caracter->GetCurrentMainState() != Character::CharacterMainState::Die)
				collisionManager_->Register(caracter->GetColliderComponent());

		}
	}

	// ヒットボックス
	for (auto& hitBoxData : hitBoxSystem->GetHitBoxData()) {
		collisionManager_->Register(hitBoxData.hitBox.get()->GetColliderComponent());
	}
	for (auto& hitBoxData : hitBoxSystem->GetLifeTimeHitBoxData()) {
		collisionManager_->Register(hitBoxData.hitBox.get()->GetColliderComponent());
	}

	// 弾のコライダー追加
	for (const auto& bullet : bulletManager->GetBullets()) {
		if (bullet->GetColliderComponent()) {
			collisionManager_->Register(bullet->GetColliderComponent());
		}
	}

	// SPポイントのコライダー追加
	for (const auto& point : specalPointManager->GetSpecalPoints()) {
		if (point->GetColliderComponent()) {
			collisionManager_->Register(point->GetColliderComponent());
		}
	}

	// 描画
	collisionManager_->DrawLine(lineCommon);
	// 判定チェック
	collisionManager_->CheckAll();
	// 動的コライダー削除
	collisionManager_->ClearDynamic();

}
