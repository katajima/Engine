#pragma once
#include "DirectXGame/application/base/Character/State/BaseMainState.h"
#include "MediumMeleeEnemySubState.h"

namespace Character {
	// 移動
/// <summary>
/// MediumMeleeEnemyMoveStateを管理・実装するクラス。
/// </summary>
	class MediumMeleeEnemyMoveState : public MoveState {
	public:
		// コンストラクタ
		MediumMeleeEnemyMoveState(BaseCharacter* enemy)
			: MoveState(enemy) {
		}


		/// <summary>
		/// 更新
		/// </summary>
		void Update(const CharacterContext& ctx) override;

		/// <summary>
		/// 終了
		/// </summary>
		void Exit() override;

		/// <summary>
		/// 初期化
		/// </summary>
		void Enter() override;
	private:
		float rootTimer_ = 5.0f;
		float timer_ = 0.0f;
	};

	/// <summary>
	/// 攻撃
	/// </summary>
	class MediumMeleeEnemyAttackState :public AttackState {
	public:
		MediumMeleeEnemyAttackState(BaseCharacter* enemy)
			: AttackState(enemy) {
		}
		/// <summary>
		/// 更新
		/// </summary>
		void Update(const CharacterContext& ctx) override;

		/// <summary>
		/// 終了
		/// </summary>
		void Exit() override;
		/// <summary>
		/// 初期化
		/// </summary>
		void Enter() override;
	private:
		std::unique_ptr<SubStateMachine<AttackSubState, BaseAttackSubState>> subStateMachine_;
	};

	/// <summary>
	/// 死亡状態
	/// </summary>
	class MediumMeleeEnemyDieState :public DieState {
	public:
		MediumMeleeEnemyDieState(BaseCharacter* enemy)
			: DieState(enemy) {
		}

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const CharacterContext& ctx) override;

		/// <summary>
		/// 終了
		/// </summary>
		void Exit() override;
		/// <summary>
		/// 初期化
		/// </summary>
		void Enter() override;

	private:
		float dieTimer_ = 2.0f;
		float timer_ = 0.0f;
	};

	/// <summary>
	/// 被弾状態
	/// </summary>
	class MediumMeleeEnemyDamageState : public DamageState {
	public:
		MediumMeleeEnemyDamageState(BaseCharacter* enemy)
			: DamageState(enemy) {
		}

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const CharacterContext& ctx) override;

		/// <summary>
		/// 終了
		/// </summary>
		void Exit() override {};
		/// <summary>
		/// 初期化
		/// </summary>
		void Enter() override {};
	private:
		float timer_ = 0.0f;
		float damageTime_ = 0.5f;
	};
}
