#include "BulletManager.h"



BulletManager::~BulletManager()
{
	// 各弾削除
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
	// リストクリア
	bullets_.clear();
}

void BulletManager::Initialize(Engine::EntityManager* entityManager,  Engine::GlobalVariables* globalVariables, Engine::Camera* camera){
	// それぞれ
	this->entityManager = entityManager;	// エンティティ
	this->globalVariables = globalVariables;	// 保存項目
	this->camera = camera;					// カメラ
}

void BulletManager::Update()
{
	// 各弾更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
	// 弾が削除されていたら消す
	bullets_.remove_if([](const std::unique_ptr<BaseBullet>& bullet) { if (bullet->IsExpired()) {
		return true;
	}
	return false;
		});

	// 各発射物更新
	for(auto& projectile : projectiles_) {
		projectile->Update();
	}

	projectiles_.remove_if([](const std::unique_ptr<Projectile::BaseProjectile>& projectile) {
		if (!projectile->GetIsAlive()) {
			return true;
		}
		return false;
		});

}

void BulletManager::Draw() {
	// 描画
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}


	for (auto& projectile : projectiles_) {
		projectile->Draw();
	}

}

void BulletManager::DrawEffect() {
	// エフェクト描画
	for (auto& bullet : bullets_) {
		bullet->DrawP();
	}

	for (auto& projectile : projectiles_) {
		projectile->DrawEffect();
	}

}

void BulletManager::Draw2D() {
	// スプライト描画
	for (auto& bullet : bullets_) {
		bullet->Draw2D();
	}


	for (auto& projectile : projectiles_) {
		projectile->Draw2D();
	}
}

void BulletManager::AddBullet(std::unique_ptr<BaseBullet> bullet){
	// 格納
	bullets_.push_back(std::move(bullet));
}

void BulletManager::AddProjectile(std::unique_ptr<Projectile::BaseProjectile> projectile) {
	// 格納
	projectiles_.push_back(std::move(projectile));
}
