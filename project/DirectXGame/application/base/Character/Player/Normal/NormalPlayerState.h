#pragma once
//#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayerState.h"
#include "DirectXGame/application/base/BaseClass/State/BaseMainState.h"



// 移動
class PlayerStateIdle : public IdleState
{
public:
	// コンストラクタ
	PlayerStateIdle(BaseCharacter* player)
		: IdleState(player) {
	}

	// 更新
	void Update() override;

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
		: MoveState(player) {}

	// 更新
	void Update() override;

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
	void Update() override;

	// 終了
	void Exit()override;

	// 初期化
	void Enter() override;

};

// 攻撃
class PlayerStateAttack :public AttackState
{
public:
	PlayerStateAttack(BaseCharacter* player)
		: AttackState(player) {}

	// 更新
	void Update() override;

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
		: SpecialState(player) {}

	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;
};

// スキル
class PlayerStateSkill : public SkillState 
{
public:
	PlayerStateSkill(BaseCharacter* player)
		: SkillState(player) {
	}
	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;

private:
	float timer_ = 0.0f;
	float changeTimer_ = 0.2f;

};

// 防御
class PlayerStateDefense :public DefenseState {
public:
	PlayerStateDefense(BaseCharacter* player)
		: DefenseState(player) {
	}
	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;

private:
	float timer_ = 0.0f;
	float defenseTimer_ = 0.25f;
	bool isDifense_ = false;
};

// 気絶
class PlayerStateFainting : public FaintingState {
public:
	PlayerStateFainting(BaseCharacter* player)
		: FaintingState(player) {
	}
	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;

private:
	float faintingTimer_ = 5.0f;
	float timer_ = 0.0f;
	Vector2 prevleftStick = {};
};