#pragma once
#include "HitData.h"
#include <DirectXGame/engine/Math/Random.h>

// 前方宣言
namespace Character{
	class ParameterComponent;
}
// 戦闘中の倍率・軽減率を扱う
/// <summary>
/// CombatStatを管理・実装するクラス。
/// </summary>
class CombatStat
{
public:
	// 戦闘用データ構造体
	struct Data {
		float attackMultiplier_ = 1.0f;		// 攻撃倍率
		float defenseMultiplier_ = 1.0f;	// 防御倍率
		float damageReduction_ = 0.0f;		// 被ダメ軽減率(0.2 = 20%)
		float criticalRate_ = 0.5f;		// クリティカル確率
		float criticalMultiplier_ = 1.5f;	// クリティカル時の倍率
	};

	// 初期化
	void Initialize(Character::ParameterComponent* base) {
		baseParams_ = base;
	}
	// 攻撃力
	float GetEffectiveAttack() const;
	// 防御力
	float GetEffectiveDefense() const;

	// 戦闘用データの取得
	Data GetData() const { return data_; }
	// 戦闘用データの参照取得
	Data& GetDataRef() { return data_; }

private:
	Data data_;	// 戦闘用データ
private:
	Character::ParameterComponent* baseParams_ = nullptr;
};

// ダメージ計算
/// <summary>
/// DamageCalculatorを管理・実装するクラス。
/// </summary>
class DamageCalculator {
public:
	// キャラクターとキャラクターによるダメージ計算
	static float ComputeDamage(
		const CombatStat& attacker,
		const CombatStat& defender,
		float skillPower
	)
	{
		float attack = attacker.GetEffectiveAttack();		// 攻撃力取得
		float defense = defender.GetEffectiveDefense();		// 防御力取得

		// ダメージ
		float rawDamage = (attack * skillPower) - (defense * 0.5f);	// 
		rawDamage = (std::max)(0.0f, rawDamage);

		// クリティカル判定
		if (Random::RandomFloat(0, 1.0f) < attacker.GetData().criticalRate_) {
			rawDamage *= attacker.GetData().criticalMultiplier_;
		}

		// 軽減率反映
		rawDamage *= (1.0f - defender.GetData().damageReduction_);

		return rawDamage;
	}

	// 武器からキャラクターによるダメージ
	static float ComputeDamageWeapon(
		const CombatStat& attacker,
		const CombatStat& defender,
		float weponDamage)
	{
		float defense = defender.GetEffectiveDefense();	// 防御力取得


		// ダメージ
		float rawDamage = (weponDamage)-(defense * 0.5f);
		rawDamage = (std::max)(0.0f, rawDamage);

		// クリティカル判定
		if (Random::RandomFloat(0, 1.0f) < attacker.GetData().criticalRate_) {
			rawDamage *= attacker.GetData().criticalMultiplier_;
		}

		// 軽減率反映
		rawDamage *= (1.0f - defender.GetData().damageReduction_);

		return rawDamage;
	}
};



/// <summary>
/// ヒットカウンター
/// </summary>
class HitCounter {
public:

	// 更新
	void Update(float dt);

	// ヒット処理
	void Hit() {
		AddHit();		// 加算
		SetResetHitTime();	// タイマーリセット
	}

	// ヒット回数がリセットされるまでの時間設定
	void SetHitTimer(float time) {
		hitData_.hitTime.maxT = time;
	}



public:
	// ヒット数の取得
	int GetHitCount() const { return hitData_.hitCount; }
	// 攻撃がヒットしたら加算
	void AddHit() { hitData_.AddHit(); };
	// ヒットタイマーのリセット
	void SetResetHitTime() { hitData_.ResetTimer(); }

public:

	// ヒットデータを取得
	AttackHitData& GetHitData() { return hitData_; }
private:
	// ヒットデータを取得
	AttackHitData hitData_{};
};
