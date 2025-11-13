#include "BulletManager.h"


#include "DirectXGame/application/base/Bullet/PlayerRangeBombingBullet.h"
#include "DirectXGame/application/base/Bullet/PlayerStanBullet.h"
#include "DirectXGame/application/base/Bullet/PlayerBullet.h"




BulletManager::~BulletManager()
{
	// 各弾削除
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
	// リストクリア
	bullets_.clear();
}

void BulletManager::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Camera* camera)
{

	// それぞれ
	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目
	camera_ = camera;					// カメラ

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

void BulletManager::GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad)
{
	std::unique_ptr<BaseBullet> bullet;

	// 弾のタイプ
	switch (type)
	{
	case BulletManager::BulletType::kPlayerMissile:	// プレイヤーミサイル
		
		break;
	case BulletManager::BulletType::kEnemyBullet:	// 敵弾

		break;
	case BulletManager::BulletType::kRangeBombingSpecial:	// 範囲攻撃弾
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		break;
	case BulletType::kPlayerStan:	// スタン弾
		bullet = std::make_unique<PlayerStanBullet>();
		break;
	default:
		break;
	}

	bullet->SetTargerRange(targetPos, rad);	// 範囲とターゲット設定
	bullet->SetPlayer(player_);	// プレイヤーセット
	bullet->SetEffect(effect_);	// エフェクトセット
	bullet->Initialize(entity3DManager_, entity2DManager_, globalVariables_, position, camera_);// 弾の初期化

	// 格納
	bullets_.push_back(std::move(bullet));
}

void BulletManager::GenerateBullet(BulletType type, const BulletInfo& info, BaseEnemy* enemy)
{
	std::unique_ptr<BaseBullet> bullet;


	switch (type)
	{
	case BulletManager::BulletType::kPlayerMissile:	// ミサイル
		break;
	case BulletManager::BulletType::kEnemyBullet: // 敵玉
		break;
	case BulletManager::BulletType::kPlayerBullet:	// プレイヤー弾
		bullet = std::make_unique<PlayerBullet>();
		break;	
	case BulletManager::BulletType::kRangeBombingSpecial:	// 範囲攻撃弾
		bullet = std::make_unique<PlayerRangeBombingBullet>();
		break;
	case BulletType::kPlayerStan:	// スタン弾
		bullet = std::make_unique<PlayerStanBullet>();
		break;
	default:
		break;
	}

	bullet->SetInfo(info); // 設定
	bullet->SetPlayer(player_);	// プレイヤーセット
	bullet->SetEffect(effect_);	// エフェクトセット
	bullet->SetEnemy(enemy);	// 敵セット
	bullet->Initialize(entity3DManager_, entity2DManager_, globalVariables_, info.position, camera_); // 弾の初期化
	
	// moveする
	bullets_.push_back(std::move(bullet));

}
