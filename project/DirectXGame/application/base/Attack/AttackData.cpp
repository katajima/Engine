#include "AttackData.h"
#include <DirectXGame/engine/Math/Random.h>

#pragma region KnockbackData

Vector3 KnockbackData::DirectionPower() const
{
	Vector3 reslut{};
	
	if (data_.type == Type::kDirection) {	// 方向
		reslut = data_.normal.Normalize();
		// y座標同士の高さが同じでもy方向に飛ばしたい場合は
		if (data_.isVerticalBoost_) {
			// yを1に
			reslut.y = 1.0f;
		}
		// それぞれかける
		reslut.x *= data_.power_;
		reslut.y *= data_.verticalBoost_;
		reslut.z *= data_.power_;
	}
	else if (data_.type == Type::kRandom) {	// ランダム
		reslut = Vector3{
			Random::RandomFloat(-data_.power_,data_.power_),
			Random::RandomFloat(-data_.power_,data_.power_),
			Random::RandomFloat(-data_.power_,data_.power_)
		};
	}


	float t = std::clamp(timer_ / data_.duration_, 0.0f, 1.0f);

	// 減衰係数
	// damping_=0 → 1.0（減衰なし）
	// damping_=1 → (1-t)（終了で完全停止）
	float dampFactor = 1.0f - (t * data_.damping_);

	reslut *= dampFactor;

	return reslut;
}

#pragma endregion // ノックバックデータ

#pragma region AirStickData

void AirStickData::Update(float dt) {
	if (world_ == nullptr) return;
	
	timer_ += dt;
	
	// 終了しているなら処理しない
	if (timer_ > data_.duration_) {
		return;
	}
	Vector3 targetPos{};

	if (data_.useWorldSpace_) {
		// すでに targetOffset_ がワールド座標基準の場合
		targetPos = data_.targetOffset_;
	}
	else {
		// 攻撃者の位置を origin としてオフセットを計算する場合
		// attackerPos_ が攻撃者の現在位置（外部からセットされる前提）
		targetPos = world_->GetWorldPosition() + data_.targetOffset_;
	}

	//-----------------------------------------
	// 2. 現在位置からターゲット位置への補正
	//-----------------------------------------
	Vector3 dir = targetPos;// -data_.currentPos_;   // 現在 → ターゲット
	float dist = dir.Length();

	if (dist > 0.0f) {
		dir.Normalize();
	}

	//-----------------------------------------
	// 3. 追従速度に応じて移動量を計算
	//-----------------------------------------
	// followSpeed_: 1フレームあたりの吸着スピード
	// stickStrength_: 吸着補正の強さ
	float moveStep = data_.followSpeed_ * dt;

	// 補間（LERP のような計算）
	Vector3 move = dir * (moveStep * data_.stickStrength_);

	//-----------------------------------------
	// 4. 移動を適用
	//-----------------------------------------
	// 終端誤差を防ぐためのクランプ
	if (move.Length() > dist) {
		//data_.currentPos_ = targetPos;
	}
	else {
		//data_.currentPos_ += move;
	}

	//-----------------------------------------
	// 5. 向き補正（攻撃者の方向へ向かせる）
	//-----------------------------------------
	if (data_.keepFacingAttacker_) {
		Vector3 lookDir = world_->GetWorldPosition();// -data_.currentPos_;
		if (lookDir.Length() > 0.0001f) {
			lookDir.Normalize();
			// Y-Up, Z-Forward の座標系なので Z を正面方向とする
			//data_.currentRot_.y = std::atan2(lookDir.x, lookDir.z);
		}
	}

	//-----------------------------------------
	// 6. 重力の ON/OFF
	//-----------------------------------------
	if (data_.gravityEnabled_) {
		//data_.gravityEnabled_ = false;
	}
}

#pragma endregion // エアスティックデータ

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
	if (count < num  && !isDamage) {
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

#pragma region 

void AttackReactionData::Update(float dt) {
	airStickData.Update(dt);// 空中・地上固定データ更新
	hitStopData.Update(dt);	// ヒットストップデータ更新
	knockback.Update(dt);	// ノックバックデータ更新
	damageData.Update(dt);	// ダメージデータ更新
}




#pragma endregion // 攻撃データ
