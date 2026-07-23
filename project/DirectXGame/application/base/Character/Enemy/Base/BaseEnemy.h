#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "EnemyData.h"

#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAttackSystem.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAiSystem.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyCrowdBehaviorData.h>
#include "EnemyWaveExitState.h"


namespace Character {
	class BasePlayer;

	/// <summary>
	/// 敵クラス
	/// </summary>
	class BaseEnemy : public BaseCharacter {
	public:
		virtual ~BaseEnemy();
		// 初期化
		virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;

		/// <summary>敵共通のオブジェクト、パラメータ、AI、攻撃、死亡処理を初期化する。</summary>
		/// <param name="inputSystem">入力参照。AI操作のみの場合はnullptrを許容する。</param>
		/// <param name="entity3DManager">敵オブジェクトとコンポーネントの生成元。</param>
		/// <param name="globalVariables">敵調整値の登録・保存先。</param>
		/// <param name="position">敵の初期ワールド座標。</param>
		/// <param name="camera">敵描画に使用する非所有カメラ。</param>
		/// <param name="modelName">使用するモデル名。</param>
		/// <param name="charaName">オブジェクトと調整値に使用する敵名。</param>
		/// <param name="colliderRadius">球コライダー半径。0より大きい値を指定する。</param>
		/// <param name="shadowSize">影の表示倍率。</param>
		void BaseInitialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
			const std::string& modelName, const std::string& charaName, float colliderRadius = 1.0f,float shadowSize = 1.0f);
		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		virtual void Update();
		/// <summary>
		/// 基盤の更新
		/// </summary>
		void BaseUpdate();
		/// <summary>
		/// 描画エフェクト
		/// </summary>
		virtual void DrawEffect(){};
		/// <summary>
		/// 描画2d
		/// </summary>
		virtual void Draw2D(){};
		/// <summary>
		/// 移動
		/// </summary>
		virtual void Move();
		/// <summary>
		/// ジャンプ
		/// </summary>
		virtual void Jump(){};
		/// <summary>
		/// リロード
		/// </summary>
		virtual void Reload() {};
		/// <summary>現在の攻撃・移動対象を取得する。</summary>
		/// <returns>非所有ポインター。対象が設定されていない場合はnullptr。</returns>
		const BaseCharacter* GetTarget();

		/// <summary>攻撃・移動対象となるキャラクターを設定する。</summary>
		/// <param name="target">対象への非所有ポインター。解除時はnullptr。</param>
		void SetTargetCharacters(BaseCharacter* target);

		/// <summary>敵固有の攻撃判断システムを取得する。</summary>
		/// <returns>BaseEnemyが所有する非所有ポインター。未初期化時はnullptr。</returns>
		EnemyAttackSystem* GetEnemyAttackSystem() const;
		/// <summary>共有敵AIシステムを取得する。</summary>
		/// <returns>CharacterManagerなどが所有する非所有ポインター。</returns>
		EnemyAiSystem* GetEnemyAiSystem() const { return enemAi; }

		/// <summary>共有敵AIシステムを設定する。</summary>
		/// <param name="aiSystem">非所有ポインター。敵の生存中は有効であること。</param>
		void SetEnemyAiSystem(EnemyAiSystem* aiSystem) { enemAi = aiSystem; }
		// ウェーブ終了による自然退場を開始する
		/// <param name="duration">自然退場に使用する秒数。</param>
		void BeginWaveExit(float duration = 1.2f);
		/// <summary>
		/// 退場中の敵は攻撃・ロックオン・撃破得点の対象から外す
		/// </summary>
		bool IsWaveExiting() const;
		/// <summary>
		/// 退場完了後に削除されても撃破と判定しないための識別
		/// </summary>
		bool IsWaveExitRemoval() const;
		/// <summary>死亡時のSPポイントを、必殺技キルでなければ生成する。</summary>
		/// <param name="position">SPポイントを出すワールド座標。</param>
		/// <param name="point">加算されるSP量。</param>
		void DropSpecialPointOnDeath(const Vector3& position, int point);

	public:
		/// <summary>
		/// ID設定
		/// </summary>
		void SetID(uint32_t id) { id_ = id; }
		/// <summary>
		/// 敵タイプ取得
		/// </summary>
		EnemyType GetType() const { return type_; };
		/// <summary>
		/// 敵タイプ設定
		/// </summary>
		void SetType(EnemyType type) { type_ = type; };
		/// <summary>
		/// 群衆グループ設定
		/// </summary>
		void SetCrowdGroupId(int groupId) { crowdGroupId_ = groupId; }
		/// <summary>
		/// 群衆グループ取得
		/// </summary>
		int GetCrowdGroupId() const { return crowdGroupId_; }
		/// <summary>
		/// 群衆内番号設定
		/// </summary>
		void SetCrowdMemberIndex(uint32_t memberIndex) { crowdMemberIndex_ = memberIndex; }
		/// <summary>
		/// 群衆内番号取得
		/// </summary>
		uint32_t GetCrowdMemberIndex() const { return crowdMemberIndex_; }
		/// <summary>
		/// この敵が所属する群衆グループの行動設定
		/// </summary>
		void SetCrowdBehavior(const CrowdBehaviorSettings& behavior) { crowdBehavior_ = behavior; }
		const CrowdBehaviorSettings& GetCrowdBehavior() const { return crowdBehavior_; }

		/// <summary>現在のターゲット座標を取得する。</summary>
		/// <returns>ターゲットのワールド座標。未設定時は既定のゼロベクトル。</returns>
		Vector3 GetTargetPos();
		/// <summary>自身から現在ターゲットまでの距離を取得する。</summary>
		/// <returns>ワールド単位の直線距離。</returns>
		float GetTargetDistance();
		/// <summary>自身からターゲットへ向かう方向を取得する。</summary>
		/// <returns>正規化済みワールド方向。方向を算出できない場合はゼロベクトル。</returns>
		Vector3 TargetDirection();

	protected:
		/// <summary>敵に追従するエフェクト用ワールド変換を初期化する。</summary>
		/// <param name="pos">初期ワールド座標。</param>
		void InitializeEffect(const Vector3& pos);
		// 武器初期化
		template<typename T>
		void InitializeWeapon(const Vector3& pos) {
			/// <summary>
			/// 武器
			/// </summary>
			weapon_ = std::make_unique<T>();
			InitializeBaseWeapon(pos);
		};
	private:
		void InitializeBaseWeapon(const Vector3& pos);
	private:

		void InitializeBaseEnemyAddItem();
		/// <summary>
		/// 更新保存項目
		/// </summary>
		void UpdateBaseEnemyGetValue();
	protected:
		EnemyType type_ = EnemyType::kMediumMelee; // 敵の種類
		uint32_t id_ = 0; // ID
		int crowdGroupId_ = 0; // 群衆グループID
		uint32_t crowdMemberIndex_ = 0; // 群衆内番号
		CrowdBehaviorSettings crowdBehavior_{}; // 群衆の行動パターン設定
	protected:
		// 高さ
		float skyHeight_ = 1.5f;
		// 止まるか
		bool isStopping_ = false;
		// グローバルデータ
		EnemyGlobalData globalData_;
		// 
		float dieScore = 100.0f;
		// 移動速度
		float moveSpeed_ = 3.0f;
	protected:
		std::unique_ptr <Engine::WorldTransform> worldEffect_ = nullptr;
		// 攻撃システム
		std::unique_ptr<EnemyAttackSystem> attackSystem_ = nullptr;	
		// ウェーブ終了専用の退場ステート
		std::unique_ptr<EnemyWaveExitState> waveExitState_ = nullptr;

		EnemyAiSystem* enemAi = nullptr;	// AIシステム
	};
}
