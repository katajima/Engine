#pragma once
#include "DirectXGame/application/base/Character/State/BaseMainState.h"


namespace Character {
	// 移動
	class PlayerStateIdle : public IdleState
	{
	public:
		// コンストラクタ
		PlayerStateIdle(BaseCharacter* player)
			: IdleState(player) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit()override;

		// 初期化
		void Enter() override;
	};

	// 移動
	class PlayerStateMove : public MoveState
	{
	public:
		// コンストラクタ
		PlayerStateMove(BaseCharacter* player)
			: MoveState(player) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit()override;

		// 初期化
		void Enter() override;
	};

	// ジャンプ
	class PlayerStateJump : public JumpState {
	public:
		// コンストラクタ
		PlayerStateJump(BaseCharacter* player)
			: JumpState(player) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit()override;

		// 初期化
		void Enter() override;
	private:
		float timer = 0.0f;
		float maxTimer = 0.1f;
	};

	// 攻撃
	class PlayerStateAttack :public AttackState
	{
	public:
		PlayerStateAttack(BaseCharacter* player)
			: AttackState(player) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;
		// 初期化
		void Enter() override;
	};

	// 必殺
	class PlayerStateSpecial :public SpecialState
	{
	public:
		PlayerStateSpecial(BaseCharacter* player)
			: SpecialState(player) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;
		// 初期化
		void Enter() override;
	};

	// 気絶
	class PlayerStateFainting : public FaintingState {
	public:
		PlayerStateFainting(BaseCharacter* player)
			: FaintingState(player) {
		}
		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;
		// 初期化
		void Enter() override;

	private:
		float faintingTimer_ = 5.0f;
		float timer_ = 0.0f;
		Vector2 prevleftStick = {};
	};
}