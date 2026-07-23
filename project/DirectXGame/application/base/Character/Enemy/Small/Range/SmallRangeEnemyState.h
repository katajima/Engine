#pragma once
#include "DirectXGame/application/base/Character/State/BaseMainState.h"


namespace Character {
	class BaseCharacter;

	// 移動
/// <summary>
/// SmallRangeEnemyMoveStateを管理・実装するクラス。
/// </summary>
	class SmallRangeEnemyMoveState : public MoveState {
	public:
		// コンストラクタ
		SmallRangeEnemyMoveState(BaseCharacter* enemy)
			: MoveState(enemy) {
		}


		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;

		// 初期化
		void Enter() override;
	private:
		float rootTimer_ = 5.0f;
		float timer_ = 0.0f;
	};

	/// <summary>
	/// 攻撃
	/// </summary>
	class SmallRangeEnemyAttackState :public AttackState	{
	public:
		SmallRangeEnemyAttackState(BaseCharacter* enemy)
			: AttackState(enemy) {
		}
		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;
		// 初期化
		void Enter() override;
	private:
		float timer_ = 0.0f;
		bool hasShot_ = false;

		float shotTime_ = 0.4f;   // 狙ってから撃つまで
		float endTime_ = 0.8f;   // 攻撃全体時間
		float turnSpeed_ = 6.0f;  // 回転速度
	};

	/// <summary>
	/// 死亡状態
	/// </summary>
	class SmallRangeEnemyDieState :public DieState {
	public:
		SmallRangeEnemyDieState(BaseCharacter* enemy)
			: DieState(enemy) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;
		// 初期化
		void Enter() override;

	private:
		float dieTimer_ = 2.0f;
		float timer_ = 0.0f;
	};

	/// <summary>
	/// 被弾状態
	/// </summary>
	class SmallRangeEnemyDamageState : public DamageState {
	public:
		SmallRangeEnemyDamageState(BaseCharacter* enemy)
			: DamageState(enemy) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override {};
		// 初期化
		void Enter() override {};
	private:
		float timer_ = 0.0f;
		float damageTime_ = 0.25f;
	};


}


