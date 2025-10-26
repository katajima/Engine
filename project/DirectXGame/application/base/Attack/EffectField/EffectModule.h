#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "DirectXGame/application/base/State/BaseMainState.h"
// 前方宣言
class BaseCharacter;

// エフェクトのモジュール基底クラス
class EffectModule {
public:
	virtual void Effect(BaseCharacter* character) = 0;
private:
};

// ダメージを与えるエフェクトモジュール
class DamageModule : public EffectModule {
public:

	// 効果を発揮する
	void Effect(BaseCharacter* character) override;

	// ダメージ量を設定する
	void SetDamageAmount(float amount) { damageAmount = amount; }
private:
	float damageAmount = 0.0f; // 与えるダメージ量
};

// ノックバックを与えるエフェクトモジュール
class KnockbackModule : public EffectModule {
public:
	// 効果を発揮する
	void Effect(BaseCharacter* character) override;

	// ノックバックデータ設定
	void SetKnockbackData(KnockbackData data) { knockback_ = data; }

	// ノックバックするか
	void SetIsKnockback(bool is) { isknockback_ = is; }
	// ノックバック持続時間設定
	void SetKnockbackTime(float time) { knockbackTime_ = time; };
private:
	// ノックバックデータ
	KnockbackData knockback_;
	float knockbackTime_ = 0.0f;
	bool isknockback_ = false;
};

// ステートを変更させるエフェクトモジュール
class ChangeStateModule : public EffectModule {
public:
	// 効果を発揮する
	void Effect(BaseCharacter* character) override;

	// 変更ステート設定
	void SetState(CharacterMainState state) { state_ = state; }

private:
	CharacterMainState state_; // 変更ステート
};


