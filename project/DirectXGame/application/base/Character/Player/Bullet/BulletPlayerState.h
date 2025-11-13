#pragma once
#include "DirectXGame/application/base/State/BaseMainState.h"



// 移動
class BulletPlayerStateIdle : public IdleState
{
public:
	// コンストラクタ
	BulletPlayerStateIdle(BaseCharacter* player)
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
class BulletPlayerStateMove : public MoveState
{
public:
	// コンストラクタ
	BulletPlayerStateMove(BaseCharacter* player)
		: MoveState(player) {
	}

	// 更新
	void Update() override;

	// 終了
	void Exit()override;

	// 初期化
	void Enter() override;
};

// ジャンプ
class BulletPlayerStateJump : public JumpState {
public:
	// コンストラクタ
	BulletPlayerStateJump(BaseCharacter* player)
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
class BulletPlayerStateAttack :public AttackState
{
public:
	BulletPlayerStateAttack(BaseCharacter* player)
		: AttackState(player) {
	}

	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;
};

// 必殺
class BulletPlayerStateSpecial :public SpecialState
{
public:
	BulletPlayerStateSpecial(BaseCharacter* player)
		: SpecialState(player) {
	}

	// 更新
	void Update() override;

	// 終了
	void Exit() override;
	// 初期化
	void Enter() override;
};

// スキル
class BulletPlayerStateSkill : public SkillState
{
public:
	BulletPlayerStateSkill(BaseCharacter* player)
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
class BulletPlayerStateDefense :public DefenseState {
public:
	BulletPlayerStateDefense(BaseCharacter* player)
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
class BulletPlayerStateFainting : public FaintingState {
public:
	BulletPlayerStateFainting(BaseCharacter* player)
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