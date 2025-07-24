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
	Light,          // 弱攻撃
	Heavy,          // 強攻撃
	Special,        // 必殺技など特殊攻撃
	JumpAttack,     // 空中攻撃
	DashAttack,     // ダッシュ中に攻撃
	Charge,         // チャージ攻撃（ボタン長押し）
	Cancel,         // コンボキャンセル（ステップや回避などに移行）
	Parry,          // 敵の攻撃に対するカウンター
};


class ComboData {
public:
	float damage;
	float staminaCost = 0;					// スタミナ消費量
	float mpCost = 0;                       // MP消費
	float movementSpeedMultiplier = 1.0f;	// 攻撃中の移動速度倍率
	KnockbackData knockbackData{};			// ノックバックデータ
};
