#pragma once
#include"DirectXGame/application/base/BaseClass/Object/BaseObject.h"

// ノックバックデータ構造体
struct KnockbackData
{
	float power = 0.0f;					// ノックバックの力
	float yPower = 0.0f;				// ノックバックY距離(敵をどれだけ上に吹き飛ばすか)
	Vector3 normal{};					// 飛ぶ方向

	// 方向とパワーを合算した値(Vector3)
	Vector3 DirectionPower() const{
		Vector3 reslut{};
		reslut = normal.Normalize();
		reslut.x *= power;
		reslut.y *= yPower;
		reslut.z *= power;


		return reslut;
	}


};

enum class AttackInput {
	Light,
	Heavy
};


class ComboData {
public:
	float damage;
	float staminaCost = 0;					// スタミナ消費量
	float mpCost = 0;                       // MP消費
	float movementSpeedMultiplier = 1.0f;	// 攻撃中の移動速度倍率
	KnockbackData knockbackData{};			// ノックバックデータ
};

//// 攻撃モーションデータ構造体
//struct ComboMotionData
//{
//	float attackInterval = 1.0f;		// 攻撃間隔
//	float currentTime = 0.0f;			// 現在の時間(攻撃開始からの時間)
//	float startupTime = 0.1f;			// 発生時間(攻撃開始から当たり判定が出るまでの時間)
//	float attackAnimationTime = 0.5f;	// 攻撃アニメーション時間(攻撃モーションの長さ)
//	float recoveryTime = 0.3f;			// 攻撃後の隙(攻撃モーション終了から次の入力を受け付けるまでの時間)
//	bool canBeInterrupted = true;		// 攻撃中にキャンセル可能かのフラグ
//
//	bool isStartup = false;				// 発生時間か
//	bool isAttackAnimation = false;		// 攻撃発生中か
//	bool isRecovery = false;			// 後隙中か
//
//
//	// 時間加算
//	void AddCurrentTime(float deltaTime) {
//		currentTime += deltaTime;
//
//		if (currentTime <= startupTime) { // 発生フレーム中
//			isStartup = true;
//			isAttackAnimation = false;
//			isRecovery = false;
//		}
//		else if (currentTime > startupTime && currentTime <= attackAnimationTime + startupTime) { // 攻撃フレーム中
//			isStartup = false;
//			isAttackAnimation = true;
//			isRecovery = false;
//		}
//		else if (currentTime > attackAnimationTime + startupTime &&  currentTime <= AllTime()) { // 後隙中 
//			isStartup = false;
//			isAttackAnimation = false;
//			isRecovery = true;
//		}
//		else {// 何もしていない
//			isStartup = false;
//			isAttackAnimation = false;
//			isRecovery = false;
//		}
//	}
//	// 時間リセット
//	void ResetTime() {
//		currentTime = 0.0f;
//	}
//	// 全体時間取得
//	float AllTime() const {
//		return startupTime + attackAnimationTime + recoveryTime;
//	}
//	// 時間設定
//	void SetTime(float startup, float attackAnimation, float recovery)
//	{
//		startupTime = startup;
//		attackAnimationTime = attackAnimation;
//		recoveryTime = recovery;
//	}
//
//
//};
