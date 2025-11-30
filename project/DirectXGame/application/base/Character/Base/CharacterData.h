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
		parameters_.stamina.useRate = true;	// スタミナ変化
		parameters_.HP.Update(deltaTime_);	// HP更新
		parameters_.MP.Update(deltaTime_);	// MP更新
		parameters_.stamina.Update(deltaTime_);	// スタミナ更新
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
	
	// HP取得
	Gage& HP() { return parameters_.HP; };				// ヒットポイント
	// MP取得
	Gage& MP() { return  parameters_.MP; };				// マジックポイント
	// スタミナ取得
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

