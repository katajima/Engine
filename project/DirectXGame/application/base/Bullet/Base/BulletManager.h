#pragma once
// 
#include "DirectXGame/application/base/Bullet/Base/BaseBullet.h"
#include "DirectXGame/application/base/Bullet/Base/BaseProjectile.h"

// 前方宣言
namespace Character {
	class BasePlayer;
	class BaseEnemy;
	class BaseCharacter;
}
namespace Engine {
	class EntityManager;
	class GlobalVariables;
	class Camera;
}


/// <summary>
/// 弾管理クラス
/// </summary>
class BulletManager {
public:
	~BulletManager();
	// 初期化
	void Initialize(Engine::EntityManager* entityManager,Engine::GlobalVariables* globalVariables, Engine::Camera* camera);
	

	// 更新
	void Update();

	// 描画3D
	void Draw();
	
	// 描画エフェクト
	void DrawEffect();

	// 描画2D
	void Draw2D();

	// 弾の追加
	void AddBullet(std::unique_ptr<BaseBullet> bullet);

	// 発射物の追加
	void AddProjectile(std::unique_ptr<Projectile::BaseProjectile> projectile);

public: // 取得or設定
	// 弾リストを取得
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const { return bullets_; }
		
	// 発射物リストを取得
	const std::list<std::unique_ptr<Projectile::BaseProjectile>>& GetProjectiles() const { return projectiles_; }

	// エフェクトの設定
	void SetEffect(EffectSystem* effect) { this->effect = effect; }
private:

	// 弾
	std::list<std::unique_ptr<BaseBullet>> bullets_;

	// 発射物
	std::list<std::unique_ptr<Projectile::BaseProjectile>> projectiles_;

private:
	EffectSystem* effect;								// 演出
	Engine::Camera* camera;								// カメラ
	Engine::GlobalVariables* globalVariables = nullptr;	// 保存項目
	Engine::EntityManager* entityManager;	// 3dオブジェクト管理
};
