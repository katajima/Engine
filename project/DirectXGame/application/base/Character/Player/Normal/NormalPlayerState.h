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

	// 回避
	class PlayerStateAvoidance : public AvoidanceState {
	public:
		// コンストラクタ
		PlayerStateAvoidance(BaseCharacter* player)
			: AvoidanceState(player) {
		}

		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;

		// 初期化
		void Enter() override;
	private:
		float timer_ = 0.0f;					// 回避経過時間
		float duration_ = 0.75f;				// 回避の継続時間
		float speed_ = 15.0f;					// 回避中の移動速度
		float staminaCost_ = 20.0f;				// 回避開始時に消費するスタミナ
		Vector3 direction_ = { 0.0f,0.0f,1.0f };	// 回避に使うワールド方向
		bool isDirectionFixed_ = false;			// 回避方向を決定済みか
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

	// 死亡
	class PlayerStateDie : public DieState {
	public:
		PlayerStateDie(BaseCharacter* player)
			: DieState(player) {
		}
		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;
		// 初期化
		void Enter() override;

	private:

	};

	// 被弾
	class PlayerStateDamage : public DamageState {
	public:
		PlayerStateDamage(BaseCharacter* player)
			: DamageState(player) {
		}
		// 更新
		void Update(const CharacterContext& ctx) override;

		// 終了
		void Exit() override;
		// 初期化
		void Enter() override;
	private:
		float timer = 0.0f;
		Vector3 hitEffectPos = { 0,0.75f,0 };

	};


}
