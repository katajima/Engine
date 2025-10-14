#pragma once
#include<string>
#include <memory>
#include <functional>
#include"DirectXGame/engine/math/MathFanctions.h"
#include "SubStateMachine.h"


// キャラクター行動(一段階)
enum class CharacterMainState {
	Move,       // 移動
	Jump,       // ジャンプ
	Avoidance,  // 回避
	Defense,    // 防御
	Attack,     // 攻撃
	Skill,      // スキル
	Special,    // 必殺技
	Die,        // 死亡
	Fainting,   // 気絶
	Damage,     // 被弾
	Dash,       // ダッシュ
};

class BaseCharacter;
class BaseCharacterState {
public:
	virtual ~BaseCharacterState() = default;
	BaseCharacterState(const CharacterMainState& state, BaseCharacter* character) :characterMainstate_(state), character_(character) {};

	virtual void Enter() {}
	virtual void Update() {}
	virtual void Exit() {}


public:
	CharacterMainState GetCharacterMainState() const { return characterMainstate_; }
protected:
	CharacterMainState characterMainstate_;
	// 操作状態
	BaseCharacter* character_ = nullptr;
};




// 移動
class MoveState : public BaseCharacterState {
public:
	MoveState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Move, character) {
	}

	virtual void Enter() {}
	virtual void Update() {}
	virtual void Exit() {}
};

// ジャンプ
class JumpState : public BaseCharacterState {
public:
	JumpState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Jump, character) {
	}

	virtual void Enter() {}
	virtual void Update() {}
	virtual void Exit() {}
};

// 回避
class AvoidanceState : public BaseCharacterState {
public:
	AvoidanceState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Avoidance, character) {
	}

	void Enter() override {
		// 無敵時間セット
		// character_->SetInvincible(true);
	}

	void Update() override {
		// 回避動作更新
		// if (アニメーション終了) → Moveへ戻す
	}

	void Exit() override {
		// character_->SetInvincible(false);
	}
};

// 防御
class DefenseState : public BaseCharacterState {
public:
	DefenseState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Defense, character) {
	}

	virtual void Enter() {}
	virtual void Update() {}
	virtual void Exit() {}
};

// 攻撃
class AttackState : public BaseCharacterState {
public:
	AttackState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Attack, character) {
	}

	virtual void Enter() {}
	virtual void Update() {}
	virtual void Exit() {}

private:
	float timer_ = 0.0f;
	float readyTime_ = 0.2f;
	float swing1Time_ = 0.4f;
	float endTime_ = 0.3f;
};

// 死亡
class DieState : public BaseCharacterState {
public:
	DieState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Die, character) {
	}

	void Enter() override {
		// character_->PlayAnimation("Die");
		// character_->SetCollidable(false);
	}

	void Update() override {
		// 死亡演出中
	}

	void Exit() override {}
};

// 被弾
class DamageState : public BaseCharacterState {
public:
	DamageState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Damage, character) {
	}

	void Enter() override {
		// character_->PlayAnimation("Damage");
	}

	void Update() override {
		// よろけ中
		// if (終了) → Moveへ
	}

	void Exit() override {}
};

// ダッシュ
class DashState : public BaseCharacterState {
public:
	DashState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Dash, character) {
	}

	void Enter() override {
		// character_->SetDashSpeed(...);
	}

	void Update() override {
		// ダッシュ中の移動処理
		// if (終了) → Moveへ
	}

	void Exit() override {}
};

// 気絶
class FaintingState : public BaseCharacterState {
public:
	FaintingState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Fainting, character) {
	}

	virtual void Enter() {}
	virtual void Update() {}
	virtual void Exit() {}


	float faintDuration_ = 3.0f; //
private:
	float faintTimer_ = 0.0f;
};

// スキル
class SkillState : public BaseCharacterState {
public:
	enum class SkillPhase {
		Charge,   // 詠唱
		Cast,     // 発動
		Cooldown  // クールダウン
	};

	SkillState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Skill, character) {
	}

	void Enter() override {
		phase_ = SkillPhase::Charge;
		timer_ = 0.0f;
		// character_->PlayAnimation("SkillCharge");
	}

	void Update() override {
		//timer_ += character_->GetDeltaTime();

		//switch (phase_) {
		//case SkillPhase::Charge:
		//    if (timer_ >= chargeTime_) {
		//        phase_ = SkillPhase::Cast;
		//        timer_ = 0.0f;
		//        // スキル効果発動
		//        // character_->PlayAnimation("SkillCast");
		//        // character_->SpawnSkillEffect();
		//    }
		//    break;

		//case SkillPhase::Cast:
		//    if (timer_ >= castTime_) {
		//        phase_ = SkillPhase::Cooldown;
		//        timer_ = 0.0f;
		//        // character_->PlayAnimation("SkillEnd");
		//    }
		//    break;

		//case SkillPhase::Cooldown:
		//    if (timer_ >= cooldownTime_) {
		//        // character_->ChangeState(CharacterMainState::Move);
		//    }
		//    break;
		//}
	}

	void Exit() override {
		// character_->StopSkillEffect();
	}

private:
	SkillPhase phase_ = SkillPhase::Charge;
	float timer_ = 0.0f;
	float chargeTime_ = 0.8f;   // 詠唱時間
	float castTime_ = 1.2f;     // 発動時間
	float cooldownTime_ = 0.5f; // 終了待機
};

// 必殺技
class SpecialState : public BaseCharacterState {
public:
	enum class Phase {
		Intro,   // カットインなどの開始演出
		Action,  // 攻撃動作・エフェクト展開
		End      // 終了
	};

	SpecialState(BaseCharacter* character)
		: BaseCharacterState(CharacterMainState::Special, character) {
	}

	virtual void Enter() {}
	virtual void Update() {}
	virtual void Exit() {}

private:
	Phase phase_ = Phase::Intro;
	float timer_ = 0.0f;
	float introTime_ = 1.0f;  // カットインなど
	float actionTime_ = 2.5f; // 攻撃アニメ長
	float endTime_ = 0.8f;    // 終了演出
};
