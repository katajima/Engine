#pragma once
// 
#include "DirectXGame/application/base/Bullet/Base/BaseBullet.h"
#include "DirectXGame/application/base/Bullet/Base/BaseProjectile.h"
#include "DirectXGame/application/base/Bullet/Base/ProjectileEditor.h"

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
	/// <summary>弾と発射物の管理、および発射物定義エディターを初期化する。</summary>
	/// <param name="entityManager">弾オブジェクトを生成するエンティティ管理。呼び出し側が所有し、利用中は有効であること。</param>
	/// <param name="globalVariables">発射物定義の登録・保存先。呼び出し側が所有する。</param>
	/// <param name="camera">弾描画に利用するカメラ。カメラ不要の場合はnullptrを許容する。</param>
	void Initialize(Engine::EntityManager* entityManager,Engine::GlobalVariables* globalVariables, Engine::Camera* camera);
	

	// 更新
	void Update();

	// 描画3D
	void Draw();
	
	// 描画エフェクト
	void DrawEffect();

	// 描画2D
	void Draw2D();

	/// <summary>従来形式の弾を管理対象へ追加する。</summary>
	/// <param name="bullet">所有権を移譲する弾。nullptrは追加しないこと。</param>
	void AddBullet(std::unique_ptr<BaseBullet> bullet);

	/// <summary>共通発射物を管理対象へ追加する。</summary>
	/// <param name="projectile">所有権を移譲する発射物。nullptrは追加しないこと。</param>
	void AddProjectile(std::unique_ptr<Projectile::BaseProjectile> projectile);

	/// <summary>保存名に対応する発射物パラメータを検索する。</summary>
	/// <param name="definitionName">発射物定義の保存名。</param>
	/// <returns>定義が存在する場合は内部データへの非所有ポインター、存在しない場合はnullptr。</returns>
	const Projectile::ProjectileParam* FindProjectileParam(const std::string& definitionName) const;
	/// <summary>登録済みの発射物定義をすべて取得する。</summary>
	/// <returns>エディターが所有する定義マップへの読み取り専用参照。</returns>
	const std::map<std::string, Projectile::ProjectileParam>& GetProjectileDefinitions() const {
		return projectileEditor_.GetDefinitions();
	}

public: // 取得or設定
	/// <summary>管理中の従来形式の弾を取得する。</summary>
	/// <returns>BulletManagerが所有する弾リストへの読み取り専用参照。</returns>
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const { return bullets_; }
		
	/// <summary>管理中の共通発射物を取得する。</summary>
	/// <returns>BulletManagerが所有する発射物リストへの読み取り専用参照。</returns>
	const std::list<std::unique_ptr<Projectile::BaseProjectile>>& GetProjectiles() const { return projectiles_; }

	/// <summary>弾の演出に利用するエフェクトシステムを設定する。</summary>
	/// <param name="effect">非所有ポインター。弾管理中は有効であること。</param>
	void SetEffect(EffectSystem* effect) { this->effect = effect; }
private:

	// 弾
	std::list<std::unique_ptr<BaseBullet>> bullets_;

	// 発射物
	std::list<std::unique_ptr<Projectile::BaseProjectile>> projectiles_;

	// 発射物定義の編集・保存
	Projectile::ProjectileEditor projectileEditor_;

private:
	EffectSystem* effect = nullptr;								// 演出
	Engine::Camera* camera = nullptr;								// カメラ
	Engine::GlobalVariables* globalVariables = nullptr;	// 保存項目
	Engine::EntityManager* entityManager = nullptr;	// 3dオブジェクト管理
};
