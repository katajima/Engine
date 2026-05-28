#pragma once
#include"DirectXGame/application/base/Bullet/Base/BaseBullet.h"
#include "DirectXGame/application/base/Bullet/Base/BaseProjectile.h"

/// <summary>
/// 弾の出現処理を扱うクラス
/// </summary>
class BulletSpawn {
public:
	~BulletSpawn() = default;

	// 初期化＋持ち主設定
	void Initialize(Character::BaseCharacter* owner, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Engine::Camera* camera, EffectSystem* effect, BulletManager* bulletManager = nullptr);

	void Generate();

	// 範囲攻撃する弾を生成(絨毯爆撃)
	void GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad);

	// 弾を生成
	void GenerateBullet(BulletType type, const BulletInfo& info, Character::BaseCharacter* target = nullptr);

	// 発射物を生成
	void GenerateProjectile(const Projectile::ProjectileSpawnInfo& spawnInfo,
		const Projectile::ProjectileParam& param, Character::BaseCharacter* target = nullptr);

	// エディターに保存された定義名から発射物を生成
	bool GenerateProjectile(const Projectile::ProjectileSpawnInfo& spawnInfo,
		const std::string& definitionName, Character::BaseCharacter* target = nullptr);

private:
	Character::BaseCharacter* owner = nullptr;
	BulletManager* bulletManager = nullptr;


private:
	EffectSystem* effect = nullptr;								// 演出
	Engine::Camera* camera = nullptr;								// カメラ
	Engine::GlobalVariables* globalVariables = nullptr;	// 保存項目
	Engine::EntityManager* entityManager = nullptr;	// 3dオブジェクト管理
};
