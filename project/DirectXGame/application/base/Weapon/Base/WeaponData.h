#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"

enum class WeaonType
{
	MELL,	// 近距離武器
	RANGED, // 遠距離武器
	Hybrid, // 例えば剣＋銃みたいな特殊系
};

// 遠近どちらにもあるパラメータ持つ武器データ構造体
struct WeaponData
{
	//float damage = 0;					// ダメージ 
	WeaonType type = WeaonType::MELL;	// 武器の種類(遠近)
	//Timer animetionTimer;				// アニメーションタイマー(攻撃アニメーションの管理用)
	bool isAutomatic = false;			// オート連射(入力しっぱなしで攻撃)可能かのフラグ
	bool isActive = false;				// 武器が有効かどうかのフラグ(攻撃中か)

	// 攻撃再発動時間
	float recastTime = 0;
	const float MaxRecastTime = 0.5f;
};

// 近距離武器専用のデータ構造体(近距離武器クラス用)
struct MeleeWeaponData
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
