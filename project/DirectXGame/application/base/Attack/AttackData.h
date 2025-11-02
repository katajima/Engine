#pragma once
#include "DirectXGame/engine/struct/Vector3.h"

// ノックバックデータ構造体
struct KnockbackData
{
	float power = 0.0f;					// ノックバックの力
	float yPower = 0.0f;				// ノックバックY距離(敵をどれだけ上に吹き飛ばすか)
	bool isYpower = false;				// Y方向にノックバックを与えるかどうか
	Vector3 normal{};					// 飛ぶ方向

	// 方向とパワーを合算した値(Vector3)
	Vector3 DirectionPower() const{
		Vector3 reslut{};
		reslut = normal.Normalize();
		reslut.x *= power;
		if (isYpower) {
			reslut.y = 1.0f;
		}
		reslut.y *= yPower;
		reslut.z *= power;
		return reslut;
	}
};

// コンボデータ
class ComboData {
public:
	float damage;							// ダメージ
	float staminaCost = 0;					// スタミナ消費量
	float mpCost = 0;                       // MP消費
	float moveSpeed = 0;					// 移動速度
	float movementSpeedMultiplier = 1.0f;	// 攻撃中の移動速度倍率
	float moveTime = 1.0f;					// コンボ時に移動する時間
	KnockbackData knockbackData{};			// ノックバックデータ
};


// コンボ受付条件クラス
class ComboCondition {
public:

	// 開始
	void Enter();

	// 更新
	void Update(float dt);

	// 終了
	void Exit();
	
	// 次のコンボに移行するか
	bool IsNextCombo() const { return isNextCombo_; };


public:
	float inputWindowStart_ = 0.1f;      // 入力受付スタート
	float inputWindowEnd_ = 0.5f;        // 入力受付エンド
private:
	float timer_ = 0.0f;				// 時間
	bool isNextCombo_ = false;			// 次のコンボに移行フラグ
};






