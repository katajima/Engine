#include "AttackData.h"
#include <DirectXGame/engine/Math/Random.h>

#pragma region DamageData

DamageDataHandle::DamageDataHandle()
	: data_(std::make_unique<One>()) {}

DamageDataHandle::DamageDataHandle(const DamageDataHandle& other)
	: data_(other.data_ ? other.data_->Clone() : std::make_unique<One>()) {}

DamageDataHandle& DamageDataHandle::operator=(const DamageDataHandle& other) {
	// 自己代入で現在の派生データを不要に作り直さない
	if (this != &other) {
		data_ = other.data_ ? other.data_->Clone() : std::make_unique<One>();
	}
	return *this;
}

DamageDataHandle::DamageDataHandle(const DamageData& data)
	: data_(data.Clone()) {}

void DamageDataHandle::SetType(DamageData::Type type) {
	// 指定されたダメージ方式に対応する派生インスタンスへ切り替える
	switch (type) {
	case DamageData::kOne:
		data_ = std::make_unique<One>();
		break;
	case DamageData::kContinuous:
		data_ = std::make_unique<Continuous>();
		break;
	case DamageData::kDuration:
		data_ = std::make_unique<Duration>();
		break;
	default:
		data_ = std::make_unique<One>();
		break;
	}
}

One& DamageDataHandle::GetOne() {
	return dynamic_cast<One&>(*data_);
}

const One& DamageDataHandle::GetOne() const {
	return dynamic_cast<const One&>(*data_);
}

Continuous& DamageDataHandle::GetContinuous() {
	return dynamic_cast<Continuous&>(*data_);
}

const Continuous& DamageDataHandle::GetContinuous() const {
	return dynamic_cast<const Continuous&>(*data_);
}

Duration& DamageDataHandle::GetDuration() {
	return dynamic_cast<Duration&>(*data_);
}

const Duration& DamageDataHandle::GetDuration() const {
	return dynamic_cast<const Duration&>(*data_);
}

#pragma endregion ダメージデータ

#pragma region One
// 更新
void One::Update(float dt) {
	// 一回ダメージは最初の更新で処理完了とする
	(void)dt;
	isFinish = true;
}
// ダメージ取得
float One::GetDamage() const {
	return damage;
}

#pragma endregion // 一回ダメージ

#pragma region Duration
// 更新
void Duration::Update(float dt) {

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
void Duration::Reset() {
	count = 0;
	isFinish = false;
}

// ダメージ取得
float Duration::GetDamage() const {
	return damage;
};

#pragma endregion // 持続ダメージ

#pragma region Continuous
// 更新
void Continuous::Update(float dt) {
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
void Continuous::Reset() {
	count = 0;
	isFinish = false;
}

// ダメージ取得
float Continuous::GetDamage() const
{
	float damage = 0;
	switch (oneHitDamageType)
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


