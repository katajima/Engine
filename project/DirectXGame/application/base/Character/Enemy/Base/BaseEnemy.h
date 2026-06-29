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

		// 基盤となる初期化
		void BaseInitialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
			const std::string& modelName, const std::string& charaName, float colliderRadius = 1.0f,float shadowSize = 1.0f);
		// 毎フレーム更新
		virtual void Update();
		// 基盤の更新
		void BaseUpdate();
		// 描画エフェクト
		virtual void DrawEffect(){};
		// 描画2d
		virtual void Draw2D(){};
		// 移動
		virtual void Move();
		// ジャンプ
		virtual void Jump(){};
		// 攻撃
		virtual void Attack(){};
		// リロード
		virtual void Reload() {};
		// ターゲット取得
		const BaseCharacter* GetTarget();

		void SetTargetCharacters(BaseCharacter* target);

		EnemyAttackSystem* GetEnemyAttackSystem() const;
		EnemyAiSystem* GetEnemyAiSystem() const { return enemAi; }

		void SetEnemyAiSystem(EnemyAiSystem* aiSystem) { enemAi = aiSystem; }
		// ウェーブ終了による自然退場を開始する
		void BeginWaveExit(float duration = 1.2f);
		// 退場中の敵は攻撃・ロックオン・撃破得点の対象から外す
		bool IsWaveExiting() const;
		// 退場完了後に削除されても撃破と判定しないための識別
		bool IsWaveExitRemoval() const;

	public:
		// ID設定
		void SetID(uint32_t id) { id_ = id; }
		// 敵タイプ取得
		EnemyType GetType() const { return type_; };
		// 敵タイプ設定
		void SetType(EnemyType type) { type_ = type; };
		// 群衆グループ設定
		void SetCrowdGroupId(int groupId) { crowdGroupId_ = groupId; }
		// 群衆グループ取得
		int GetCrowdGroupId() const { return crowdGroupId_; }
		// 群衆内番号設定
		void SetCrowdMemberIndex(uint32_t memberIndex) { crowdMemberIndex_ = memberIndex; }
		// 群衆内番号取得
		uint32_t GetCrowdMemberIndex() const { return crowdMemberIndex_; }
		// この敵が所属する群衆グループの行動設定
		void SetCrowdBehavior(const CrowdBehaviorSettings& behavior) { crowdBehavior_ = behavior; }
		const CrowdBehaviorSettings& GetCrowdBehavior() const { return crowdBehavior_; }

		// 対象の位置(プレイヤー)
		Vector3 GetTargetPos();
		// 対象との距離(プレイヤー)
		float GetTargetDistance();
		// ターゲット方向取得
		Vector3 TargetDirection();

	protected:
		// エフェクト初期化
		void InitializeEffect(const Vector3& pos);
		// 武器初期化
		template<typename T>
		void InitializeWeapon(const Vector3& pos) {
			// 武器
			weapon_ = std::make_unique<T>();
			InitializeBaseWeapon(pos);
		};
	private:
		void InitializeBaseWeapon(const Vector3& pos);
	private:

		void InitializeBaseEnemyAddItem();
		// 更新保存項目
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
