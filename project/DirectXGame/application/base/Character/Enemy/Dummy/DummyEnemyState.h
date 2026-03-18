#pragma once

#pragma once
#include "DirectXGame/application/base/Character/State/BaseMainState.h"


namespace Character {
	class BaseCharacter;

	// 移動
	class DummyEnemyMoveState : public MoveState
	{
	public:
		// コンストラクタ
		DummyEnemyMoveState(BaseCharacter* enemy)
			: MoveState(enemy) {
		}


		// 更新
		void Update(const CharacterContext& ctx) override {};

		// 終了
		void Exit() override {};

		// 初期化
		void Enter() override {};
	private:
		float rootTimer_ = 5.0f;
		float timer_ = 0.0f;
	};

	/// <summary>
	/// 死亡状態
	/// </summary>
	class DummyEnemyDieState :public DieState
	{
	public:
		DummyEnemyDieState(BaseCharacter* enemy)
			: DieState(enemy) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override {};

		// 終了
		void Exit() override {};
		// 初期化
		void Enter() override {};

	private:
		float dieTimer_ = 2.0f;
		float timer_ = 0.0f;
	};
}
