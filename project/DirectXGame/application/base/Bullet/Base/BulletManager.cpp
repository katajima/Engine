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

void BulletManager::Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Engine::Camera* camera){
	// それぞれ
	this->entity3DManager = entity3DManager;	// エンティティ3d
	this->entity2DManager = entity2DManager;	// エンティティ2d
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
}

void BulletManager::Draw()
{
	// 描画
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void BulletManager::DrawEffect()
{
	// エフェクト描画
	for (auto& bullet : bullets_) {
		bullet->DrawP();
	}
}

void BulletManager::Draw2D()
{
	// スプライト描画
	for (auto& bullet : bullets_) {
		bullet->Draw2D();
	}
}

void BulletManager::AddBullet(std::unique_ptr<BaseBullet> bullet){
	// 格納
	bullets_.push_back(std::move(bullet));
}
