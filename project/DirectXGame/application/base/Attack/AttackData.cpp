#include "AttackData.h"
#include <DirectXGame/engine/Math/Random.h>

#pragma region DamageData
// 更新
void DamageData::Update(float dt) {
	switch (type)
	{
	case DamageData::kOne:
		one.Update(dt);
		break;
	case DamageData::kContinuous:
		continuous.Update(dt);
		break;
	case DamageData::kDuration:
		duration.Update(dt);
		break;
	default:
		break;
	}
};
// ダメージ取得
float DamageData::GetDamage() {
	float damage = 0;

	switch (type)
	{
	case DamageData::kOne:
		damage = one.GetDamage();
		break;
	case DamageData::kContinuous:
		damage = continuous.GetDamage();
		break;
	case DamageData::kDuration:
		damage = duration.GetDamage();
		break;
	default:
		break;
	}

	return damage;
};
// 終了しているか
bool DamageData::IsFinish() {
	bool is = true;
	switch (type)
	{
	case DamageData::kOne:
		is = one.IsFinish();
		break;
	case DamageData::kContinuous:
		is = continuous.IsFinish();
		break;
	case DamageData::kDuration:
		is = duration.IsFinish();
		break;
	default:
		break;
	}
	return is;
}
// ダメージが発生しているか
bool DamageData::IsAttack()
{
	bool is = true;
	switch (type)
	{
	case DamageData::kOne:
		is = one.IsAttack();
		break;
	case DamageData::kContinuous:
		is = continuous.IsAttack();
		break;
	case DamageData::kDuration:
		is = duration.IsAttack();
		break;
	default:
		break;
	}
	return is;
}

#pragma endregion ダメージデータ

#pragma region One
// 更新
void DamageData::One::Update(float dt) {
	isFinish = true;
}
// ダメージ取得
float DamageData::One::GetDamage() const {
	return damage;
}

#pragma endregion // 一回ダメージ

#pragma region Duration
// 更新
void DamageData::Duration::Update(float dt) {

	// カウント内なら時間を加算
	if (count < num && !isDamage) {
		timer += dt;
	}
	else {	// カウントが回数以上になったら攻撃判定なし
		isDamage = false;
		isFinish = true;
	}

	if (interval >= timer) {
		timer = 0;
		count++;
		isDamage = true;
	}

}

// カウントリセット
void DamageData::Duration::Reset() {
	count = 0;
	isFinish = false;
}

// ダメージ取得 
float DamageData::Duration::GetDamage() const {
	return damage;
};

#pragma endregion // 持続ダメージ

#pragma region Continuous
// 更新
void DamageData::Continuous::Update(float dt) {
	// カウント内なら時間を加算
	if (count < num) {
		timer += dt;
	}
	else {	// カウントが回数以上になったら攻撃判定なし
		isDamage = false;
		isFinish = true;
	}

	if (interval >= timer) {
		timer = 0;
		count++;
		isDamage = true;
	}
};

// カウントリセット
void DamageData::Continuous::Reset() {
	count = 0;
	isFinish = false;
}

// ダメージ取得
float DamageData::Continuous::GetDamage() const
{
	float damage = 0;
	switch (oneHitDamegeType)
	{
	case DamageData::kConstant:	// 一定
		damage = this->damage;
		break;
	case DamageData::kCustom:	// カスタム
		damage = damages[count];
		break;
	default:
		break;
	}
	return damage;
}
#pragma endregion // 連撃ダメージ


