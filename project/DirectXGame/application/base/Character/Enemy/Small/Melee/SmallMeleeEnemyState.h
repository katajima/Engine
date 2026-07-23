#pragma once
#include "DirectXGame/application/base/Character/State/BaseMainState.h"
#include "SmallMeleeEnemySubState.h"

namespace Character {
	class BaseCharacter;

	// 移動
/// <summary>
/// SmallMeleeEnemyMoveStateを管理・実装するクラス。
/// </summary>
	class SmallMeleeEnemyMoveState : public MoveState {
	public:
		// コンストラクタ
		SmallMeleeEnemyMoveState(BaseCharacter* enemy)
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
	class SmallMeleeEnemyAttackState :public AttackState {
	public:
		SmallMeleeEnemyAttackState(BaseCharacter* enemy)
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
		float timer_ = 0.0f;
	};

	/// <summary>
	/// 死亡状態
	/// </summary>
	class SmallMeleeEnemyDieState :public DieState {
	public:
		SmallMeleeEnemyDieState(BaseCharacter* enemy)
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
	class SmallMeleeEnemyDamageState : public DamageState {
	public:
		SmallMeleeEnemyDamageState(BaseCharacter* enemy)
			: DamageState(enemy) {
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
		void Enter() override {};
	private:
		float timer_ = 0.0f;
		float damageTime_ = 0.25f;
	};

}
