#pragma once
#include<string>
#include <memory>
#include <functional>
#include "SubStateMachine.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

namespace Character {
	//前方宣言
	class BaseCharacter;

	/// <summary>
	/// キャラクターステートクラス
	/// </summary>
	class BaseCharacterState {
	public:
		virtual ~BaseCharacterState() = default;
		BaseCharacterState(const CharacterMainState& state, BaseCharacter* character) :characterMainstate_(state), character(character) {};
		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}


	public:
		/// <summary>
		/// キャラクターメインステート取得
		/// </summary>
		CharacterMainState GetCharacterMainState() const { return characterMainstate_; }
	protected:
		CharacterMainState characterMainstate_;
		// 操作状態
		BaseCharacter* character = nullptr;
	};


	// 待機
/// <summary>
/// IdleStateを管理・実装するクラス。
/// </summary>
	class IdleState : public BaseCharacterState {
	public:
		IdleState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Idle, character) {
		}
		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// 移動
/// <summary>
/// MoveStateを管理・実装するクラス。
/// </summary>
	class MoveState : public BaseCharacterState {
	public:
		MoveState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Move, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// ジャンプ
/// <summary>
/// JumpStateを管理・実装するクラス。
/// </summary>
	class JumpState : public BaseCharacterState {
	public:
		JumpState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Jump, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// 回避
/// <summary>
/// AvoidanceStateを管理・実装するクラス。
/// </summary>
	class AvoidanceState : public BaseCharacterState {
	public:
		AvoidanceState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Avoidance, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// 防御
/// <summary>
/// DefenseStateを管理・実装するクラス。
/// </summary>
	class DefenseState : public BaseCharacterState {
	public:
		DefenseState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Defense, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// 攻撃
/// <summary>
/// AttackStateを管理・実装するクラス。
/// </summary>
	class AttackState : public BaseCharacterState {
	public:
		AttackState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Attack, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}

	private:
		float timer_ = 0.0f;
		float readyTime_ = 0.2f;
		float swing1Time_ = 0.4f;
		float endTime_ = 0.3f;
	};

	// 死亡
/// <summary>
/// DieStateを管理・実装するクラス。
/// </summary>
	class DieState : public BaseCharacterState {
	public:
		DieState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Die, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// 被弾
/// <summary>
/// DamageStateを管理・実装するクラス。
/// </summary>
	class DamageState : public BaseCharacterState {
	public:
		DamageState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Damage, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// ダッシュ
/// <summary>
/// DashStateを管理・実装するクラス。
/// </summary>
	class DashState : public BaseCharacterState {
	public:
		DashState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Dash, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}
	};

	// 気絶
/// <summary>
/// FaintingStateを管理・実装するクラス。
/// </summary>
	class FaintingState : public BaseCharacterState {
	public:
		FaintingState(BaseCharacter* character)
			: BaseCharacterState(CharacterMainState::Fainting, character) {
		}

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}


		float faintDuration_ = 3.0f; //
	private:
		float faintTimer_ = 0.0f;
	};

	// スキル
/// <summary>
/// SkillStateを管理・実装するクラス。
/// </summary>
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

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}

	private:
		SkillPhase phase_ = SkillPhase::Charge;
		float timer_ = 0.0f;
		float chargeTime_ = 0.8f;   // 詠唱時間
		float castTime_ = 1.2f;     // 発動時間
		float cooldownTime_ = 0.5f; // 終了待機
	};

	// 必殺技
/// <summary>
/// SpecialStateを管理・実装するクラス。
/// </summary>
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

		/// <summary>
		/// 開始
		/// </summary>
		virtual void Enter() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(const CharacterContext& ctx) {}
		/// <summary>
		/// 終了
		/// </summary>
		virtual void Exit() {}

	private:
		Phase phase_ = Phase::Intro;
		float timer_ = 0.0f;
		float introTime_ = 1.0f;  // カットインなど
		float actionTime_ = 2.5f; // 攻撃アニメ長
		float endTime_ = 0.8f;    // 終了演出
	};
}