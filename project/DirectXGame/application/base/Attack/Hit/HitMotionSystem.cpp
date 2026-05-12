#include "HitMotionSystem.h"
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include <DirectXGame/application/base/Character/Base/CharacterData.h>
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"

#pragma region HitMotion

void HitMotionSystem::Initialize(Character::BaseCharacter* owner, EffectSystem* effectSystem) {
	this->owner = owner;
	this->effectSystem = effectSystem;
	moveRequestSystem = owner->GetMoveComponent()->GetMoveRequestSystem();

	timer_ = 0.0f;
	hitStunTimer_ = 0.0f;
	downTimer_ = 0.0f;
	isAction_ = false;
	hitMotionState_ = HitMotionState::None;

	// エフェクト座標初期化
	worldEffect_.Initialize();
	worldEffect_.parent_ = &owner->GetWorldTransform();
	worldEffect_.translate_ = { 0,1,0 };

}

void HitMotionSystem::Update(float dt) {
	DamageProcess(dt, owner->GetCharacterParameterComponent());

	// セルフヒットストップ時間カウントダウン
	GetIsTime(dt, selfHitStopTime_);

	if (!isAction_) {
		return;
	}


	// ヒットストップ中は何もしない
	if(GetIsTime(dt , hitStopTime_)) return;

	timer_ += dt;

	// ヒットスタン時間カウントダウン
	GetIsTime(dt, hitStunTimer_);
	
	// ダウン時間カウントダウン
	GetIsTime(dt, downTimer_);


	// リアクション移動の有効時間中だけ毎フレームリクエストを送る
	if (timer_ < data_.duration) {
		Vector3 velocity = BuildMoveVelocity();
		SendReactionMoveRequest(velocity * dt);
	}

	// Down 状態への遷移
	if (timer_ >= data_.duration) {
		if (data_.downTime > 0.0f) {
			hitMotionState_ = HitMotionState::Down;
		}
	}

	// 全終了判定
	if (timer_ >= data_.duration &&
		hitStunTimer_ <= 0.0f &&
		downTimer_ <= 0.0f) {
		FinishReaction();
	}
}

// ヒットエミッター（パーティクル）
void HitMotionSystem::EmitHitEffect() {
	// エフェクト座標更新
	worldEffect_.Update();
	Vector3 pos = worldEffect_.worldMat_.GetWorldPosition();

	for (auto& effect : data_.hitEffectNames) {
		if (!effect.effectName.empty()) {
			effectSystem->Emit(effect.effectName, pos);
		}
	}
}

// ヒットモーション設定
void HitMotionSystem::SetReactionData(const HitReactionData& data) {
	data_ = data;

	timer_ = 0.0f;
	hitStunTimer_ = data_.hitStunTime;	// ヒットスタン時間
	downTimer_ = data_.downTime;		// ダウン時間
	launchFloatTime_ = data_.launchFloatTime;	// 打ち上げ時間　
	hitStopTime_ = data_.targetHitStopTime;			// ヒットストップ時間
	isAction_ = true;

	switch (data_.type) {
	case HitReactionType::Knockback:
		hitMotionState_ = HitMotionState::Knockback;
		break;

	case HitReactionType::BlowAway:
		hitMotionState_ = HitMotionState::BlowAway;
		break;

	case HitReactionType::Launch:
		hitMotionState_ = HitMotionState::Launch;
		break;

	case HitReactionType::WallBounce:
		hitMotionState_ = HitMotionState::WallBounce;
		break;

	default:
		hitMotionState_ = HitMotionState::None;
		break;
	}

	DamageMotion damageMotion;
	damageMotion.SetData(data_.damageData);
	damageMotions_.push_back(damageMotion);
}

// 終了したか
bool HitMotionSystem::IsFinished() const {
	return !isAction_ &&
		hitStunTimer_ <= 0.0f &&
		downTimer_ <= 0.0f;
}

// ヒットモーション中か
bool HitMotionSystem::IsHitMotion() const {
	return isAction_;
}

// スタン中か
bool HitMotionSystem::IsHitStun() const {
	return hitStunTimer_ > 0.0f;
}

// ダウン中か
bool HitMotionSystem::IsDown() const {
	return hitMotionState_ == HitMotionState::Down;
}

// ヒットストップ中か
bool HitMotionSystem::IsHitStop() const {
	return hitStopTime_ > 0.0f;
}

bool HitMotionSystem::IsSelfHitStop() const {
	return selfHitStopTime_ > 0.0f;
}

// 重力は有効化
bool HitMotionSystem::IsGravityEnabled() const {
	return data_.gravityEnabled;
}

// 重力強度
float HitMotionSystem::GetGravityScale() const {
	return data_.gravityScale;
}

// 速度
Vector3 HitMotionSystem::BuildMoveVelocity() const {
	Vector3 dir = NormalizeSafe(data_.normal);

	// 基本は地面に沿う方向で使う
	Vector3 horizontalDir = dir;
	horizontalDir.y = 0.0f;
	horizontalDir = NormalizeSafe(horizontalDir);

	Vector3 velocity{};

	switch (data_.type) {
	case HitReactionType::Knockback:
	{
		// 軽く後方へ押す
		velocity = horizontalDir * data_.power;
		break;
	}

	case HitReactionType::BlowAway:
	{
		// 前方＋上方向
		velocity = horizontalDir * data_.power;
		if (data_.isVerticalBoost) {
			velocity.y = data_.verticalBoost;
		}
		break;
	}
	case HitReactionType::Launch:
	{
		// 打ち上げは上方向主体
		velocity = horizontalDir * (data_.power * 0.25f);

		// launchFloatTime の間は上方向を強める
		if (timer_ < data_.launchFloatTime) {
			velocity.y = data_.verticalBoost;
		}
		else {
			// その後は上方向を切る
			velocity.y = 0.0f;
		}
		break;
	}
	case HitReactionType::WallBounce:
	{
		// 壁バウンド自体の反射はここではなく衝突側で処理する想定
		velocity = horizontalDir * data_.power;
		if (data_.isVerticalBoost) {
			velocity.y = data_.verticalBoost;
		}
		break;
	}
	default:
		break;
	}

	return velocity;
}

// 
Vector3 HitMotionSystem::NormalizeSafe(const Vector3& v) const {
	const float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq <= 0.000001f) {
		return Vector3{ 0.0f, 0.0f, 1.0f };
	}
	return v.Normalize();
}

// 移動リクエスト
void HitMotionSystem::SendReactionMoveRequest(const Vector3& velocity) {
	if (!moveRequestSystem) {
		return;
	}

	MoveRequest request{};
	request.layer = MoveLayer::kBase;
	request.speedMultiplier = 1.0f;
	request.velocity = velocity;
	request.direction = NormalizeSafe(data_.normal);
	request.priority = 100;
	request.invincible = false;
	request.isLanding = false;
	request.groundHeight = 0.0f;

	moveRequestSystem->SetRequest(request);
}

// 終了
void HitMotionSystem::FinishReaction() {
	isAction_ = false;
	timer_ = 0.0f;
	hitStunTimer_ = 0.0f;
	downTimer_ = 0.0f;
	hitMotionState_ = HitMotionState::None;
}

bool HitMotionSystem::GetIsTime(float dt, float& time) {

	if (time > 0.0f) {
		time -= dt;
		if (time < 0.0f) {
			time = 0.0f;
		}
		return true;
	}

	return false;
}

#pragma endregion // ヒットモーション

#pragma region MyRegion
// ダメージ
void HitMotionSystem::DamageProcess(float dt, Character::ParameterComponent* parameter) {

	for (auto& damage : damageMotions_) {
		damage.Update(dt);

		if (damage.GetDamageData().IsAttack()) {
			parameter->parameters->HP.value -= damage.GetDamageData().GetDamage();
		}

	}

	// 終了していたら消す
	damageMotions_.remove_if([](const DamageMotion& damage) { if (!damage.IsPlaying()) {
		return true;
	}
	return false;
		});
}
#pragma endregion