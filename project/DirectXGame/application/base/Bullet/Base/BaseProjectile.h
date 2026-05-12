#pragma once
#include "ProjectileData.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Effect/Effect.h"
namespace Character {
	class BaseCharacter;
	class BaseEnemy;
	class BasePlayer;
}


namespace Projectile {

	// 発射物の基底クラス
	class BaseProjectile : public IHitReceiver{
	public:

		// 初期化
		void Initialize(Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, EffectSystem* effectSystem, const ProjectileSpawnInfo& spawnInfo,
			const ProjectileParam& param);

		// 更新
		void Update();
		// 描画
		void Draw();
		// 描画エフェクト
		void DrawEffect();
		// 描画2D
		void Draw2D();
	public:
		// 生きているか
		bool GetIsAlive() const { return isAlive_; }
		// コライダーコンポーネント取得
		Engine::ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); }
	private:
		// コライダの作成
		void CreateCollision();
		// コライダー処理
		void CollisionProcess(Engine::ColliderComponent* otherComponent,Engine::Collider* self, Engine::Collider* other);
		// 衝突処理
		void ProjectileHit();
		// 移動処理更新
		void UpdateMovement(float dt);
		// エフェクト処理更新
		void UpdateEffect(float dt);
		// 削除(終了)処理
		void DeleteProcess();
	private:
		// ワールドトランスフォーム取得
		Engine::WorldTransform& GetWorldTransform() { return objectComponent_->GetWorldTransform(); }
	private:
		// 基盤の初期化
		std::unique_ptr<ObjectComponent> objectComponent_ = nullptr;
		// 貫通カウント
		int pierceCount = 0;
		// 跳ね返りカウント
		int bounceCount = 0;
		// 生存時間
		float lifeTime = 0.0f;
		// 生成フラグ
		bool isAlive_ = true;
		// 速度
		Vector3 velocity{};
		// 方向
		Vector3 direction{};
	private:
		// 発射物のパラメータ
		ProjectileParam param_{};
		// 発射情報
		ProjectileSpawnInfo spawnInfo_{};
	private:
		// 所有者(攻撃者)
		Character::BaseCharacter* owner = nullptr;
		// ターゲット
	 	const Character::BaseCharacter* target = nullptr;
		// エフェクトシステム
		EffectSystem* effectSystem = nullptr;
	};

}