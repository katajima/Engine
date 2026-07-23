#pragma once

#include "DirectXGame/application/base/Bullet/Base/BaseProjectile.h"
#include "DirectXGame/application/base/Bullet/Base/BulletData.h"

/// <summary>
/// 敵用の標準発射物
/// </summary>
class EnemyBullet : public Projectile::BaseProjectile {
public:
	// 旧BulletInfoをBaseProjectile用の生成情報とパラメーターへ変換して初期化する
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
		EffectSystem* effectSystem, const Projectile::ProjectileSpawnInfo& spawnInfo, const BulletInfo& info);

protected:
	/// <summary>
	/// 回避成功と旧敵弾のダメージ処理を維持する
	/// </summary>
	void OnHitPlayer(Character::BasePlayer* player) override;
};
