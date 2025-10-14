#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include "optional"
#include <DirectXGame/engine/struct/Structs.h>
#include <DirectXGame/engine/Math/Random.h>

// キャラクターの基本パラメータを定義する構造体
struct BasicParameters 
{
	Gage HP;				// ヒットポイント
	Gage MP;				// マジックポイント
	Gage stamina;			// スタミナ
	float strength = 1.0f;	// 力
	float speed = 1.0f;		// 速さ
	float defense = 1.0f;	// 防御力
	float jampPower = 180.0f;// ジャンプ力
	int intelligence = 1;	// 知力
};



// キャラクターの状態
enum class CharacterState 
{
	Idle,		// 待機
	Walk,		// 歩き
	Run,		// 走り
	Dash,		// ダッシュ
	Jump,		// ジャンプ
	Fall,		// 落ちている
	Attack,		// 攻撃
	Defense,	// 防御
	Damage,		// 被弾
	Dead,		// 死亡
	Stan,		// 気絶中
};

// キャラクターの種類を定義する列挙型
enum class CharacterType
{
	None,		// キャラクターなし
	Player,		// プレイヤーキャラクター
	Enemy,		// 敵キャラクター
	NPC,		// NPCキャラクター
	Boss,		// ボスキャラクター
	Friendly,	// フレンドリーキャラクター
};

// キャラクターの性格を定義する列挙型
enum class Personality
{
	kNormal,			// 特になし
	kAggressive,		// 攻撃的
	kDefensive,			// 防御的
	kCautious,			// 用心深い
	kCurious,			// 好奇心旺盛
	kFriendly,			// フレンドリー
	kObsession			// 執着的
};

// 基本的な振るまい
enum class BasicBehavior {
	kRoot,			// 通常状態
	kAttack,		// 攻撃中
	kDie,			// 死亡状態
	kSpecialAttack,	// 特殊攻撃中
};

// 通常時の振るまい
enum class RootBehavior {
	kIdle,		// 待機状態
	kMove,		// 移動状態
	kJump,		// ジャンプ状態
};


// 敵キャラクターの種類を定義する列挙型
enum class EnemyType
{
	kNormal,	// 通常
	kBullet,	// 弾撃ち
	kAttacker,	// アタッカー 
	kTank,		// タンク
	kElite,     // エリート
	kBoss,		// ボス 
};

// プレイヤーキャラクターの種類を定義する列挙型
enum class PlayerType
{
	kNormal,	// 通常
	kBullet,	// 弾撃ち
	kAttacker,	// アタッカー 
	kTank,		// タンク
};


// キャラクターパラメータコンポーネント
class CharacterParameterComponent
{
public:
	// 更新
	void Update() {


		parameters_.stamina.useRate = true;

		parameters_.HP.Update(deltaTime_);
		parameters_.MP.Update(deltaTime_);
		parameters_.stamina.Update(deltaTime_);
	}


public:
	// HP取得
	float GetHP() const { return parameters_.HP.value; }
	// MP取得
	float GetMP() const { return parameters_.MP.value; }
	// スタミナ取得
	float GetStamina() const { return parameters_.stamina.value; }
	// パワー取得
	float GetStrength() const { return parameters_.strength; }
	// 防御力取得
	float GetDefense() const { return parameters_.defense; }
	// 知力取得
	int GetIntelligence() const { return parameters_.intelligence; }
	// 速度取得
	float GetSpeed() const { return parameters_.speed; }
	
	// スタミナがあるか
	bool IsGetStamina() const {
		return !parameters_.stamina.IsEmpty();
	}

	// 
	

	Gage& HP() { return parameters_.HP; };				// ヒットポイント
	Gage& MP() { return  parameters_.MP; };				// マジックポイント
	Gage& Stamina() { return parameters_.stamina; };	// スタミナ


	
	// 性格取得
	Personality GetPersonality() const { return personality; }
	// キャラクター種類
	CharacterType GetCharacterType() const { return characterType_; }
public:
	Personality personality = Personality::kNormal;
	CharacterType characterType_ = CharacterType::None;	// キャラクターの種類
	BasicParameters parameters_;						// 基本パラメータ

	float deltaTime_ = 1.0f / 60.0f;

};



// 戦闘中の倍率・軽減率を扱うコンポーネント
struct CombatStatComponent
{
public:
	void Initialize(CharacterParameterComponent* base) {
		baseParams_ = base;
	}
	float GetEffectiveAttack() const {
		return baseParams_->GetStrength() * attackMultiplier_;
	}

	float GetEffectiveDefense() const {
		return baseParams_->GetDefense() * defenseMultiplier_;
	}

	float attackMultiplier_ = 1.0f;		// 攻撃倍率
	float defenseMultiplier_ = 1.0f;	// 防御倍率
	float damageReduction_ = 0.0f;		// 被ダメ軽減率(0.2 = 20%)
	float criticalRate_ = 0.5f;		// クリティカル確率
	float criticalMultiplier_ = 1.5f;	// クリティカル時の倍率
private:
	CharacterParameterComponent* baseParams_ = nullptr;
};

// ダメージ計算
class DamageCalculator {
public:
	// キャラクターとキャラクターによるダメージ計算
	static float ComputeDamage(
		const CombatStatComponent& attacker,
		const CombatStatComponent& defender,
		float skillPower
	)
	{
		float attack = attacker.GetEffectiveAttack();
		float defense = defender.GetEffectiveDefense();

		float rawDamage = (attack * skillPower) - (defense * 0.5f);
		rawDamage = (std::max)(0.0f, rawDamage);

		// クリティカル判定
		if (Random::RandomFloat(0,1.0f) < attacker.criticalRate_) {
			rawDamage *= attacker.criticalMultiplier_;
		}

		// 軽減率反映
		rawDamage *= (1.0f - defender.damageReduction_);

		return rawDamage;
	}

	// 武器からキャラクターによるダメージ
	static float ComputeDamageWeapon(
		const CombatStatComponent& attacker,
		const CombatStatComponent& defender,
		float weponDamage) 
	{
		float defense = defender.GetEffectiveDefense();



		float rawDamage = (weponDamage) - (defense * 0.5f);
		rawDamage = (std::max)(0.0f, rawDamage);

		// クリティカル判定
		if (Random::RandomFloat(0, 1.0f) < attacker.criticalRate_) {
			rawDamage *= attacker.criticalMultiplier_;
		}

		// 軽減率反映
		rawDamage *= (1.0f - defender.damageReduction_);

		return rawDamage;
	}


};


// キャラクターの状態のコンポーネント
class CharacterStateComponent
{
public:

	// 移動可能
	bool CanMove() const {
		return currentState_ != CharacterState::Attack && currentState_ != CharacterState::Damage;
	}

	// 攻撃可能
	bool CanAttack() const {
		return currentState_ == CharacterState::Idle || currentState_ == CharacterState::Run || currentState_ == CharacterState::Walk || currentState_ == CharacterState::Jump;
	}

	// 変更
	void ChangeState(CharacterState state) {
		if (currentState_ != state) {
			currentState_ = state;
		}
	}

	void Update(const Vector3& velocity, bool isGrounded, bool isDead) {
		if (isDead) {
			ChangeState(CharacterState::Dead);
		}
		else if (!isGrounded && velocity.y < 0) {
			ChangeState(CharacterState::Fall);
		}
		else if (!isGrounded && velocity.y > 0) {
			ChangeState(CharacterState::Jump);
		}
		else if (velocity.Normalize().Length() > 0.5f) {
			ChangeState(CharacterState::Run);
		}
		else {
			ChangeState(CharacterState::Idle);
		}
	}
// 状態取得
	bool IsJumping() const { return currentState_ == CharacterState::Jump; }
	bool IsFalling() const { return currentState_ == CharacterState::Fall; }
	bool IsInAir() const { return IsJumping() || IsFalling(); }
	bool IsDead() const { return currentState_ == CharacterState::Dead; }
	CharacterState GetState() const { return currentState_; }
private:
	CharacterState currentState_ = CharacterState::Idle; // キャラクターの状況
};
