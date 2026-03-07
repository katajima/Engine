#pragma once
#include"DirectXGame/application/base/Bullet/Base/BaseBullet.h"

/// <summary>
/// 弾の出現処理を扱うクラス
/// </summary>
class BulletSpawn {
public:
	~BulletSpawn() = default;

	// 初期化＋持ち主設定
	void Initialize(Character::BaseCharacter* owner, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Engine::Camera* camera, Effect* effect);

	void Generate();

	// 範囲攻撃する弾を生成(絨毯爆撃)
	void GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad);

	// 弾を生成
	void GenerateBullet(BulletType type, const BulletInfo& info, Character::BaseCharacter* target = nullptr);
private:
	Character::BaseCharacter* owner = nullptr;
	BulletManager* bulletManager = nullptr;


private:
	Effect* effect = nullptr;								// 演出
	Engine::Camera* camera = nullptr;								// カメラ
	Engine::GlobalVariables* globalVariables = nullptr;	// 保存項目
	Engine::EntityManager* entityManager = nullptr;	// 3dオブジェクト管理
};