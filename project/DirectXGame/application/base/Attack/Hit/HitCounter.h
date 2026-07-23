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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Character::ParameterComponent* base) {
		baseParams_ = base;
	}
	/// <summary>
	/// 攻撃力
	/// </summary>
	float GetEffectiveAttack() const;
	/// <summary>
	/// 防御力
	/// </summary>
	float GetEffectiveDefense() const;

	/// <summary>
	/// 戦闘用データの取得
	/// </summary>
	Data GetData() const { return data_; }
	/// <summary>
	/// 戦闘用データの参照取得
	/// </summary>
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

		/// <summary>
		/// ダメージ
		/// </summary>
		float rawDamage = (attack * skillPower) - (defense * 0.5f);	// 
		rawDamage = (std::max)(0.0f, rawDamage);

		/// <summary>
		/// クリティカル判定
		/// </summary>
		if (Random::RandomFloat(0, 1.0f) < attacker.GetData().criticalRate_) {
			rawDamage *= attacker.GetData().criticalMultiplier_;
		}

		/// <summary>
		/// 軽減率反映
		/// </summary>
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


		/// <summary>
		/// ダメージ
		/// </summary>
		float rawDamage = (weponDamage)-(defense * 0.5f);
		rawDamage = (std::max)(0.0f, rawDamage);

		/// <summary>
		/// クリティカル判定
		/// </summary>
		if (Random::RandomFloat(0, 1.0f) < attacker.GetData().criticalRate_) {
			rawDamage *= attacker.GetData().criticalMultiplier_;
		}

		/// <summary>
		/// 軽減率反映
		/// </summary>
		rawDamage *= (1.0f - defender.GetData().damageReduction_);

		return rawDamage;
	}
};



/// <summary>
/// ヒットカウンター
/// </summary>
class HitCounter {
public:

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// ヒット処理
	/// </summary>
	void Hit() {
		AddHit();		// 加算
		SetResetHitTime();	// タイマーリセット
	}

	/// <summary>
	/// ヒット回数がリセットされるまでの時間設定
	/// </summary>
	void SetHitTimer(float time) {
		hitData_.hitTime.maxT = time;
	}



public:
	/// <summary>
	/// ヒット数の取得
	/// </summary>
	int GetHitCount() const { return hitData_.hitCount; }
	/// <summary>
	/// 攻撃がヒットしたら加算
	/// </summary>
	void AddHit() { hitData_.AddHit(); };
	/// <summary>
	/// ヒットタイマーのリセット
	/// </summary>
	void SetResetHitTime() { hitData_.ResetTimer(); }

public:

	/// <summary>
	/// ヒットデータを取得
	/// </summary>
	AttackHitData& GetHitData() { return hitData_; }
private:
	// ヒットデータを取得
	AttackHitData hitData_{};
};
