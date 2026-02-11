#pragma once
#include"DirectXGame/application/base/Bullet/Base/BaseBullet.h"



// 前方宣言
namespace Character {
	class BaseCharacter;
}
class BulletManager;
namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
	class GlobalVariables;
	class Camera;
}

/// <summary>
/// 弾の出現処理を扱うクラス
/// </summary>
class BulletSpawn {
public:
	~BulletSpawn() = default;

	// 初期化＋持ち主設定
	void Initialize(Character::BaseCharacter* owner, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
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
	Effect* effect;								// 演出
	Engine::Camera* camera;								// カメラ
	Engine::GlobalVariables* globalVariables = nullptr;	// 保存項目
	Engine::Entity3DManager* entity3DManager;	// 3dオブジェクト管理
	Engine::Entity2DManager* entity2DManager;  // 2Dオブジェクト管理
};