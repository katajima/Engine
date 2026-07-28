#include "HitMotionSystem.h"
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include <DirectXGame/application/base/Character/Base/CharacterData.h>
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/engine/Offscreen/PostEffect.h"
#include "DirectXGame/engine/Offscreen/PostEffectBlock.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include <DirectXGame/application/base/Object/ObjectComponent.h>
#include "DirectXGame/engine/Math/MathFunctions.h"

#pragma region HitMotion

HitMotionSystem::~HitMotionSystem(){
	// カメラエフェクト
	if (vignettePass_ && owner->GetCharacterType() == Character::Type::Player) {
		vignettePass_->SetUse(false);
	}
}

void HitMotionSystem::Initialize(Character::BaseCharacter* owner, EffectSystem* effectSystem, Engine::Camera* camera) {
	this->owner = owner;
	this->effectSystem = effectSystem;
	this->camera = camera;
	moveRequestSystem = owner->GetMoveComponent()->GetMoveRequestSystem(); 
	timer_ = 0.0f;
	hitStunTimer_ = 0.0f;
	downTimer_ = 0.0f;
	launchStartHeight_ = 0.0f;
	isAction_ = false;
	hitMotionState_ = HitMotionState::None;

	// エフェクト座標初期化
	worldEffect_.Initialize();
	worldEffect_.parent_ = &owner->GetWorldTransform();
	worldEffect_.translate_ = { 0,1,0 };

	// ビネットエフェクト初期化
	if (owner->GetCharacterType() == Character::Type::Player) {
		ConfigureVignette();
	}
}

void HitMotionSystem::Update(float dt) {
	// ダメージ処理
	DamageProcess(dt, owner->GetCharacterParameterComponent());


	// セルフヒットストップ時間カウントダウン
	GetIsTime(dt, selfHitStopTime_);

	// レンダーテクスチャエフェクト処理
	RenderTargetEffectProcess(dt);

	// マテリアルの色を元に戻す時間カウントダウン
	MaterialColorProcess(dt);

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

	// 打ち上げ時間カウントダウン
	GetIsTime(dt, launchFloatTime_);


	// リアクション移動の有効時間中だけ毎フレームリクエストを送る
	if (timer_ < data_.duration) {
		Vector3 velocity = BuildMoveVelocity();
		Vector3 movement = velocity * dt;
		if (data_.type == HitReactionType::Launch && movement.y > 0.0f) {
			const float maxHeight = launchStartHeight_ + data_.launchMaxHeight;
			const float remainingHeight = maxHeight - owner->GetWorldTransform().GetWorldPosition().y;
			if (remainingHeight <= 0.0f) {
				movement.y = 0.0f;
			}
			else if (movement.y > remainingHeight) {
				movement.y = remainingHeight;
			}
		}
		SendReactionMoveRequest(movement);
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
		downTimer_ <= 0.0f && 
		launchFloatTime_ <= 0.0f) {
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
	data_.duration;
	hitStunTimer_ = data_.hitStunTime;	// ヒットスタン時間
	downTimer_ = data_.downTime;		// ダウン時間
	launchFloatTime_ = data_.launchFloatTime;	// 打ち上げ時間　
	launchStartHeight_ = owner->GetWorldTransform().GetWorldPosition().y;
	hitStopTime_ = data_.targetHitStopTime;			// ヒットストップ時間
	renderTargetEffectTime_ = data_.renderTargetEffectTime; // レンダーテクスチャエフェクト時間
	timerForVignette_ = data_.renderTargetEffectTime;		// ビネットエフェクト時間
	materialColorTime_ = 0.25f;
	materialColorTimeMax_ = 0.25f;
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

	case HitReactionType::Suction:
		hitMotionState_ = HitMotionState::Suction;
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
bool HitMotionSystem::IsFinished() const { return !IsHitMotion(); }

// デバッグ用に現在のリアクションを即終了する
void HitMotionSystem::ForceFinishReaction() { FinishReaction(); }

// ヒットモーション中か
bool HitMotionSystem::IsHitMotion() const { return isAction_; }

// スタン中か
bool HitMotionSystem::IsHitStun() const { return hitStunTimer_ > 0.0f; }

// ダウン中か
bool HitMotionSystem::IsDown() const { return hitMotionState_ == HitMotionState::Down; }

bool HitMotionSystem::IsLaunch() const { return launchFloatTime_ > 0.0f; }

// ヒットストップ中か
bool HitMotionSystem::IsHitStop() const { return hitStopTime_ > 0.0f; }

bool HitMotionSystem::IsSelfHitStop() const { return selfHitStopTime_ > 0.0f; }

// 重力は有効化
bool HitMotionSystem::IsGravityEnabled() const { return data_.gravityEnabled; }

// 重力強度
float HitMotionSystem::GetGravityScale() const { return data_.gravityScale; }

// 速度
Vector3 HitMotionSystem::BuildMoveVelocity() const {
	Vector3 dir = Math::NormalizeSafe(data_.normal);

	// 基本は地面に沿う方向で使う
	Vector3 horizontalDir = dir;
	horizontalDir.y = 0.0f;
	horizontalDir = Math::NormalizeSafe(horizontalDir);

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
	case HitReactionType::Suction:
	{
		// 吸い付きはヒットボックス中心へ向かう3D方向をそのまま使う
		velocity = dir * data_.power;
		if (data_.isVerticalBoost) {
			velocity.y += data_.verticalBoost;
		}
		break;
	}
	default:
		break;
	}

	return velocity;
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
	request.direction = Math::NormalizeSafe(data_.normal);
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
	launchFloatTime_ = 0.0f;
	launchStartHeight_ = 0.0f;
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
	// パラメータ未初期化時はダメージ処理を行わない。
	if (!parameter) {
		return;
	}

	for (auto& damage : damageMotions_) {
		damage.Update(dt);

		if (damage.GetDamageData().IsAttack()) {
			// リアクションデータの攻撃属性を渡し、死亡時の報酬判定に使えるようにする。
			owner->ApplyAttackDamage(damage.GetDamageData().GetDamage(), data_.attribute);
		}

	}

	// 終了していたら消す
	damageMotions_.remove_if([](const DamageMotion& damage) { if (!damage.IsPlaying()) {
		return true;
	}
	return false;
		});
}

void HitMotionSystem::RenderTargetEffectProcess(float dt){
	// カメラエフェクト
	if (vignettePass_ && owner->GetCharacterType() == Character::Type::Player) {
		// レンダーテクスチャエフェクト時間カウントダウン
		if (GetIsTime(dt, renderTargetEffectTime_)) {
			vignettePass_->SetUse(true);
			auto* vignetteData = vignettePass_->GetPostEffectData()->GetVignette()->Data();
			vignetteData->scale = 9.5f * (timerForVignette_ / renderTargetEffectTime_);
			vignetteData->squared = 0.55f;
			vignetteData->color = { 1,0,0,renderTargetEffectTime_ / timerForVignette_ };
		}
		else {
			vignettePass_->SetUse(false);
		}

	}
}

void HitMotionSystem::MaterialColorProcess(float dt){
	if (GetIsTime(dt, materialColorTime_) && owner->GetCharacterType() == Character::Type::Enemy) {
		owner->GetObjectComponent()->SetColor({1,0,0,1});
	}
	else {
		owner->GetObjectComponent()->SetColor({1,1,1,1});
	}
}

void HitMotionSystem::ConfigureVignette() {
	if (!camera) return;

	Engine::PostEffectPipeline* pipeline = camera->GetPostEffectPipeline();
	if (!pipeline) {
		return;
	}

	// 画面端を締めるビネットを追加し、回避成功の瞬間を強調する
	vignettePass_ = pipeline->FindPass(kVignettePassName_);
	if (!vignettePass_) {
		camera->AddEffectBlock(kVignetteEffectName_, Engine::PostEffectBlockType::kVignette, false);
		vignettePass_ = pipeline->FindPass(kVignettePassName_);
	}

	if (!vignettePass_ || !vignettePass_->GetPostEffectData() ||
		!vignettePass_->GetPostEffectData()->GetVignette()) {
		return;
	}

	// scaleは端の暗さの広がり、squaredは落ち方の鋭さとして扱う
	auto* vignetteData = vignettePass_->GetPostEffectData()->GetVignette()->Data();
	vignetteData->scale = 9.5f;
	vignetteData->squared = 0.55f;
	vignetteData->color = { 1,0,0,1 };
}
#pragma endregion
