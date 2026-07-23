#pragma once

#include "DirectXGame/application/base/Bullet/Base/BaseProjectile.h"
#include "DirectXGame/application/base/Bullet/Base/BulletData.h"

/// <summary>
/// プレイヤー用の標準発射物
/// </summary>
class PlayerBullet : public Projectile::BaseProjectile {
public:
	// 旧BulletInfoをBaseProjectile用の生成情報とパラメーターへ変換して初期化する
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
		EffectSystem* effectSystem, const Projectile::ProjectileSpawnInfo& spawnInfo, const BulletInfo& info);

protected:
	/// <summary>
	/// 敵へのダメージに加えて、プレイヤーのヒット数とコンボ命中を通知する
	/// </summary>
	void OnHitEnemy(Character::BaseEnemy* enemy) override;
};
