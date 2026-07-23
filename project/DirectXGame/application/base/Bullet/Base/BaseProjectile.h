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
/// <summary>
/// BaseProjectileを管理・実装するクラス。
/// </summary>
	class BaseProjectile : public IHitReceiver{
	public:
		/// <summary>
		/// 派生発射物を基底ポインターから安全に破棄する
		/// </summary>
		virtual ~BaseProjectile() = default;

		/// <summary>発射物の見た目、移動、衝突、演出を生成情報と定義値から初期化する。</summary>
		/// <param name="entity3DManager">発射物オブジェクトを生成するエンティティ管理。</param>
		/// <param name="globalVariables">調整値の登録・参照先。</param>
		/// <param name="effectSystem">移動・命中演出の出力先。演出不要の場合はnullptrを許容する。</param>
		/// <param name="spawnInfo">生成位置、方向、所有者、ターゲットを含む生成情報。</param>
		/// <param name="param">寿命、速度、当たり判定などを含む発射物定義。</param>
		void Initialize(Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, EffectSystem* effectSystem, const ProjectileSpawnInfo& spawnInfo,
			const ProjectileParam& param);

		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// 描画エフェクト
		/// </summary>
		void DrawEffect();
		/// <summary>
		/// 描画2D
		/// </summary>
		void Draw2D();
	public:
		/// <summary>発射物が管理対象として生存しているか取得する。</summary>
		/// <returns>更新・描画対象ならtrue、寿命切れまたは命中で削除待ちならfalse。</returns>
		bool GetIsAlive() const { return isAlive_; }
		/// <summary>衝突登録に使用するコライダーコンポーネントを取得する。</summary>
		/// <returns>内部ObjectComponentが所有する非所有ポインター。</returns>
		Engine::ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); }
	protected:
		/// <summary>プレイヤー命中時に派生発射物固有の処理を実行する。</summary>
		/// <param name="player">命中したプレイヤー。命中通知中のみ有効。</param>
		virtual void OnHitPlayer(Character::BasePlayer* player);
		/// <summary>敵命中時に派生発射物固有の処理を実行する。</summary>
		/// <param name="enemy">命中した敵。命中通知中のみ有効。</param>
		virtual void OnHitEnemy(Character::BaseEnemy* enemy);
		/// <summary>この発射物を生成したキャラクターを取得する。</summary>
		/// <returns>生成者への非所有ポインター。生成者を指定していない場合はnullptr。</returns>
		Character::BaseCharacter* GetOwner() const { return owner; }
		/// <summary>初期化時に保持した発射物定義を取得する。</summary>
		/// <returns>発射物の生存期間中有効な定義への読み取り専用参照。</returns>
		const ProjectileParam& GetParam() const { return param_; }
	private:
		/// <summary>
		/// コライダの作成
		/// </summary>
		void CreateCollision();
		/// <summary>コライダー通知を解釈し、命中対象に応じた処理へ振り分ける。</summary>
		/// <param name="otherComponent">衝突相手のコンポーネント。存在しない場合はnullptr。</param>
		/// <param name="self">自身のコライダー。</param>
		/// <param name="other">衝突相手のコライダー。</param>
		void CollisionProcess(Engine::ColliderComponent* otherComponent,Engine::Collider* self, Engine::Collider* other);
		/// <summary>
		/// 衝突処理
		/// </summary>
		void ProjectileHit();
		/// <summary>設定された移動方式に従って座標と速度を更新する。</summary>
		/// <param name="dt">秒単位のフレーム時間。</param>
		void UpdateMovement(float dt);
		/// <summary>移動中エフェクトとトレイルを更新する。</summary>
		/// <param name="dt">秒単位のフレーム時間。</param>
		void UpdateEffect(float dt);
		/// <summary>
		/// 削除(終了)処理
		/// </summary>
		void DeleteProcess();
	private:
		/// <summary>
		/// ワールドトランスフォーム取得
		/// </summary>
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
