#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include "optional"
// AI Character Data
// キャラクターの思考や知能を持つデータ

// ゲージを管理するための構造体
struct Gage {
	float value = 0;		// ゲージの値
	float maxValue = 0;		// ゲージの最大値
	float minValue = 0;		// ゲージの最小値
	float defaultValue = 0;	// ゲージの初期値
	float rateFluctuation = 1.0f;		// ゲージの変化率（増減速度（秒間））
	bool useRate = false;	// ゲージの変化率を使用するかどうか


	// 初期値、最小値、最大値、変化率を設定する関数
	void Initiaize(float defaultV, float min,float max,float rate){
		defaultValue = defaultV;	// 初期値を設定
		value = defaultValue;		// ゲージの値を初期値に設定
		maxValue = max;				// 最大値を設定
		minValue = min;				// 最小値を設定
		rateFluctuation = rate;			// 変化率を設定
	}

	// ゲージも値を加算する関数
	void Add(float addValue) {
		value += addValue;			// ゲージの値に追加する
		if (value > maxValue) value = maxValue; // 最大値を超えないように制限
		if (value < minValue) value = minValue; // 最小値を下回らないように制限
	}

	// 最大値と現在のゲージの値を比較して、ゲージが満タンかどうかを返す関数
	bool IsFull() const {
		return value >= maxValue; // ゲージの値が最大値以上なら満タン
	}

	// 最大値と現在のゲージの値を比較して、ゲージが空かどうかを返す関数
	bool IsEmpty() const {
		return value <= minValue; // ゲージの値が最小値以下なら空
	}

	// 最大値と現在のゲージの値を比較して、比率を返す関数
	float GetRate() const {
		if (maxValue == 0) return 0.0f; // 最大値が0の場合は比率を0にする
		return std::abs(value / maxValue);
	}

	// ゲージを更新する関数
	void Update(float delta) {
		if (useRate) {
			if(rateFluctuation != 0.0f)
			value += delta * rateFluctuation;
		}
		if (value > maxValue) value = maxValue;
		if (value < minValue) value = minValue;
	}
};

// キャラクターの基本パラメータを定義する構造体
struct BasicParameters 
{
	Gage HP;				// ヒットポイント
	Gage MP;				// マジックポイント
	Gage stamina;			// スタミナ
	float strength = 1.0f;	// 力
	float speed = 1.0f;		// 速さ
	float defense = 1.0f;	// 防御力
	int intelligence = 1;	// 知力
};

// キャラクターの状況を定義する構造体
struct Situation
{
	bool isAttacking = false;	// 攻撃中フラグ
	bool isDefending = false;	// 防御中フラグ
	bool isMoving = false;		// 移動中フラグ
	bool isIdle = true;			// 待機中フラグ
	bool isStunned = false;		// 気絶中フラグ
	bool isInvincible = false;	// 無敵フラグ
	bool isJumping = false;		// ジャンプ中フラグ
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

// キャラクターデータの基底クラス
class CharacterData
{
public:
	void Initialize() {}	// 初期化関数


public:
	Situation situation_;			// キャラクターの状況
	CharacterType characterType_ = CharacterType::None;	// キャラクターの種類
	BasicParameters parameters_;	// 基本パラメータ
};