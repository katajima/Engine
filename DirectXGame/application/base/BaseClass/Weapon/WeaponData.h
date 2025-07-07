#pragma once
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"
#include "DirectXGame/application/base/BaseClass/Attack/AttackData.h"

enum class WeaonType
{
	MELL,	// 近距離武器
	RANGED, // 遠距離武器
	Hybrid, // 例えば剣＋銃みたいな特殊系
};

// 攻撃方法
enum class  AttackTypePlay
{
	kNone = 0,
	kNormal, // 通常攻撃
	kJump, // ジャンプ攻撃
};

struct AttackKeyFlag
{
	bool IsNormalAttack; // B
};

// コンボデータ構造体
struct ComboData 
{
	bool isComboWeapon = true;			// コンボ武器かのフラグ(連続攻撃可能かどうか)
	int comboMaxCount = 3;				// コンボの最大回数
	int currentComboCount = 0;			// 現在のコンボ回数
	bool isComboNext = false;			// 次のコンボを受け付けるかのフラグ(コンボ間隔内かどうか)

	// コンボの現在の回数をリセット
	void ResetCurrentComboCount() {	currentComboCount = 0; }
	// コンボの現在の回数をインクリメント
	void IncrementCurrentComboCount() { currentComboCount++; }
};



struct AttackMotions 
{
	std::string name = "";					// 名前
	float damage = 0;						// ダメージ 
	float staminaCost = 0;					// スタミナ消費量
	float movementSpeedMultiplier = 1.0f;	// 攻撃中の移動速度倍率(攻撃中の移動速度を変化させる)
	KnockbackData knockbackData{};			// ノックバックデータ
	ComboMotionData motionData{};			// モーションデータ
	Vector3 initRotate{};					// 回転初期位置
	Vector3	rotateSpeed{};					// 回転更新(速度)	
};

// 遠近どちらにもあるパラメータ持つ武器データ構造体
struct WeaponData
{
	float damage = 0;					// ダメージ 
	WeaonType type = WeaonType::MELL;	// 武器の種類(遠近)
	Timer animetionTimer;				// アニメーションタイマー(攻撃アニメーションの管理用)
	bool isAutomatic = false;			// オート連射(入力しっぱなしで攻撃)可能かのフラグ
	bool isActive = false;				// 武器が有効かどうかのフラグ(攻撃中か)
	KnockbackData knockbackData{};		// ノックバックデータ
	ComboMotionData motionData{};		// モーションデータ
	ComboData comboData{};				// コンボデータ
	float staminaCost = 0;				// スタミナ消費量
	float movementSpeedMultiplier = 1.0f;	// 攻撃中の移動速度倍率(攻撃中の移動速度を変化させる)
};

// 攻撃ヒットデータ構造体
struct AttackHitData
{
	int hitCount = 0;	// ヒットカウント
	Timer hitTime;		// コンボリセットまでの時間

	void SetmaxHitTime(float maxHitTime)
	{
		hitTime.maxT = maxHitTime; // コンボリセットまでの時間を設定
	}

	// タイマーをリセット
	void ResetTimer()
	{
		hitTime.t = 0; // タイマーをリセット
	}

	// リセット
	void Reset()
	{
		hitCount = 0;
		hitTime.t = 0;
	}

	// ヒットカウントを増やす
	void AddHit()
	{
		hitCount++;
	}

	// 更新
	void Update(float deltaTime)
	{
		hitTime.Update(deltaTime);

		if (hitTime.IsMaxOverT()) {
			Reset(); // タイマーが最大値を超えたらリセット
		}

	}

};

// 近距離武器専用のデータ構造体(近距離武器クラス用)
struct MellWeaponData
{
	bool isCharging = false;				// チャージ中かのフラグ
	float chargeTime = 0.0f;				// チャージ時間(チャージ開始からの時間)
	float chargeDamageMultiplier = 1.0f;	// チャージによるダメージ倍率(チャージ時間に応じて変化する)
	float hitStopTime = 0.0f;				// ヒットストップ時間(攻撃が当たった時の一時的な停止時間)
	float comboInterval = 0.5f;				// コンボ間隔(次の攻撃を受け付けるまでの時間)
	float comboDamageMultiplier = 1.0f;		// コンボによるダメージ倍率(コンボ回数に応じて変化する)
	float comboResetTime = 0.5f;			// コンボリセット時間(コンボが途切れた時にリセットされるまでの時間)
};

// 遠距離武器専用のデータ構造体(遠距離武器クラス用)
struct RangedWeaponData
{
	float bulletSpeed = 10.0f;				// 弾の速度
	float bulletLifeTime = 1.0f;			// 弾のライフタイム(弾が消えるまでの時間)
	float bulletSpread = 0.0f;				// 弾の散布率(弾がばらける範囲)
	float reloadTime = 1.0f;				// リロード時間(弾を補充するのにかかる時間)
	bool isHoming = false;					// ホーミング弾かのフラグ(敵に自動で追尾するかどうか)
	float homingStrength = 0.5f;			// ホーミングの強さ(敵にどれだけ引き寄せられるか)
	int ammoCapacity = 10;					// 弾薬の最大容量(一度に持てる弾の数)
	int currentAmmo = 10;					// 現在の弾薬数(残りの弾の数)
	bool hasPierce = false;					// 貫通弾かのフラグ(敵を貫通するかどうか)
};
