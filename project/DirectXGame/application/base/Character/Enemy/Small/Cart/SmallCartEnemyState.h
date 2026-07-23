#pragma once
#include "DirectXGame/application/base/Character/State/BaseMainState.h"



namespace Character {
	class BaseCharacter;

	// 移動
/// <summary>
/// SmallCartEnemyMoveStateを管理・実装するクラス。
/// </summary>
	class SmallCartEnemyMoveState : public MoveState
	{
	public:
		// コンストラクタ
		SmallCartEnemyMoveState(BaseCharacter* enemy)
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
	/// 死亡状態
	/// </summary>
	class SmallCartEnemyDieState :public DieState
	{
	public:
		SmallCartEnemyDieState(BaseCharacter* enemy)
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
}
