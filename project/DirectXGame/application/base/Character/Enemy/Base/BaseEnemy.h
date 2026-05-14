#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "EnemyData.h"
#include "DirectXGame/application/base/Character/Vision/VisionComponent.h"

#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAttackSystem.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAiSystem.h>


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
			const std::string& modelName, const std::string& charaName, float colliderRadius = 1.0f);
		// 毎フレーム更新
		virtual void Update() = 0;
		// 基盤の更新
		void BaseUpdate();
		// 描画エフェクト
		virtual void DrawEffect() = 0;
		// 描画2d
		virtual void Draw2D() = 0;
		// 移動
		virtual void Move() = 0;
		// ジャンプ
		virtual void Jump() = 0;
		// 攻撃
		virtual void Attack() = 0;
		// ターゲット取得
		const BaseCharacter* GetTarget();

		void SetTargetCharacters(BaseCharacter* target);

		EnemyAttackSystem* GetEnemyAttackSystem() const;

		void SetEnemyAiSystem(EnemyAiSystem* aiSystem) { enemAi = aiSystem; }

	public:
		// ID設定
		void SetID(uint32_t id) { id_ = id; }
		// 敵タイプ取得
		EnemyType GetType() const { return type_; };
		// 敵タイプ設定
		void SetType(EnemyType type) { type_ = type; };

		// 対象の位置(プレイヤー)
		Vector3 GetTargetPos();
		// 対象との距離(プレイヤー)
		float GetTargetDistance();
		// ターゲット方向取得
		Vector3 TargetDirection();

	private:

		void InitializeBaseEnemyAddItem();
		// 更新保存項目
		void UpdateBaseEnemyGetValue();
		//
		void InitShadowObjectComponent(const std::string& charaName);

	protected:
		EnemyType type_ = EnemyType::kMediumMelee; // 敵の種類
		uint32_t id_ = 0; // ID
	protected:
		//
		bool isStopping_ = false;
		// グローバルデータ
		EnemyGlobalData globalData_;
		float dieScore = 100.0f;
	protected:
		std::unique_ptr <Engine::WorldTransform> worldEffect_ = nullptr;
		// 攻撃システム
		std::unique_ptr<EnemyAttackSystem> attackSystem_ = nullptr;	

		EnemyAiSystem* enemAi = nullptr;	// AIシステム
	};
}