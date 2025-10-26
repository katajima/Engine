#include "BulletManager.h"


#include "DirectXGame/application/base/Bullet/PlayerRangeBombingBullet.h"
#include "DirectXGame/application/base/Bullet/PlayerStanBullet.h"



BulletManager::~BulletManager()
{
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
	bullets_.clear();
}

void BulletManager::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	camera_ = camera;

}

void BulletManager::Update()
{
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
	bullets_.remove_if([](const std::unique_ptr<BaseBullet>& bullet) { if (bullet->IsExpired()) {
		return true;
	}
	return false;
		});
}

void BulletManager::Draw()
{
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void BulletManager::DrawEffect()
{
	for (auto& bullet : bullets_) {
		bullet->DrawP();
	}
}

void BulletManager::Draw2D()
{
	for (auto& bullet : bullets_) {
		bullet->Draw2D();
	}
}

void BulletManager::GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad)
{
	std::unique_ptr<BaseBullet> bullet;


	switch (type)
	{
	case BulletManager::BulletType::kPlayerMissile:
		
		break;
	case BulletManager::BulletType::kEnemyBullet:
		break;
	case BulletManager::BulletType::kRangeBombingSpecial:
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		break;
	case BulletType::kPlayerStan:
		bullet = std::make_unique<PlayerStanBullet>();
		break;
	default:
		break;
	}

	bullet->SetTargerRange(targetPos, rad);	// 範囲とターゲット設定
	bullet->SetPlayer(player_);	// プレイヤーセット
	bullet->SetEffect(effect_);	// エフェクトセット
	bullet->Initialize(entity3DManager_, entity2DManager_, globalVariables_, position, camera_);// 弾の初期化


	bullets_.push_back(std::move(bullet));
}

void BulletManager::GenerateBullet(BulletType type, Vector3 position, BaseEnemy* enemy)
{
	std::unique_ptr<BaseBullet> bullet;


	switch (type)
	{
	case BulletManager::BulletType::kPlayerMissile:
		break;
	case BulletManager::BulletType::kEnemyBullet:
		break;
	case BulletManager::BulletType::kRangeBombingSpecial:
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		break;
	case BulletType::kPlayerStan:
		bullet = std::make_unique<PlayerStanBullet>();
		break;
	default:
		break;
	}

	bullet->SetPlayer(player_);	// プレイヤーセット
	bullet->SetEffect(effect_);	// エフェクトセット
	bullet->SetEnemy(enemy);	// 敵セット
	bullet->Initialize(entity3DManager_, entity2DManager_, globalVariables_, position, camera_); // 弾の初期化
	
	// moveする
	bullets_.push_back(std::move(bullet));

}
