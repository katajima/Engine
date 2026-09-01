#include "PlayerSubWeapon.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include <algorithm>
#include <cmath>

void PlayerSubWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
	this->inputSystem = inputSystem;						// インプット
	this->entityManager = entityManager; // エンティティ3d
	this->globalVariables = globalVariables;				// グローバル変数
	this->camera = camera;								// カメラ

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entityManager, globalVariables, "PlayerSubWeapon", "AnimatedCube.gltf", false, false, this);
	objectComponent_->SetSRT({ 0.15f,0.15f,0.15f }, { 0.0f,0.0f,0.0f }, position);	// SRT設定

	// 描画する
	GetObject3D()->SetIsDraw(true);
}

void PlayerSubWeapon::Update() {
	// dt指定が無い呼び出しでは待機位置だけを同期する
	UpdateIdle();
	// ワールドトランスフォーム更新
	GetObject3D()->UpdateWorldTransform();
}

void PlayerSubWeapon::Update(float dt) {
	// 状態ごとの移動処理
	switch (throwState_) {
	case ThrowState::kIdle:
		UpdateIdle();
		break;
	case ThrowState::kThrow:
		UpdateThrow(dt);
		break;
	case ThrowState::kHold:
		UpdateHold(dt);
		break;
	case ThrowState::kReturn:
		UpdateReturn(dt);
		break;
	default:
		break;
	}

	// ワールドトランスフォーム更新
	GetObject3D()->UpdateWorldTransform();
}

void PlayerSubWeapon::Throw(const Vector3& startPosition, const Vector3& direction) {
	// 既に投擲中なら二重発射しない
	if (throwState_ != ThrowState::kIdle) {
		return;
	}

	// 投擲方向は水平寄りに正規化して、ゼロ方向なら前方へ投げる
	throwDirection_ = direction;
	throwDirection_.y = 0.0f;
	if (throwDirection_.LengthSq() <= 0.0001f) {
		throwDirection_ = { 0.0f,0.0f,1.0f };
	}
	throwDirection_ = throwDirection_.Normalize();

	// 投擲開始位置と時間を初期化する
	throwTimer_ = 0.0f;
	throwState_ = ThrowState::kThrow;
	throwStartPosition_ = startPosition;
	GetWorldTransform().translate_ = startPosition;

	// 必要なら投擲方向にサブ武器の向きを合わせる
	if (throwData_.alignToDirection) {
		GetWorldTransform().rotate_ = throwData_.rotateOffset;
		GetWorldTransform().rotate_.y += std::atan2(throwDirection_.x, throwDirection_.z);
	}
}

void PlayerSubWeapon::StopAtCurrentPosition() {
	// 投擲中の位置で止め、効果発動や回収待ちに使う
	if (throwState_ == ThrowState::kThrow) {
		throwState_ = ThrowState::kHold;
		throwTimer_ = 0.0f;
	}
}

void PlayerSubWeapon::Recall() {
	// 待機中以外なら現在位置から所有者の近くへ戻す
	if (throwState_ != ThrowState::kIdle && throwState_ != ThrowState::kReturn) {
		returnStartPosition_ = GetWorldTransform().translate_;
		throwTimer_ = 0.0f;
		throwState_ = ThrowState::kReturn;
	}
}

bool PlayerSubWeapon::IsThrowing() const {
	// 投擲または戻りの間はサブ武器攻撃中として扱う
	return throwState_ != ThrowState::kIdle;
}

bool PlayerSubWeapon::IsHolding() const {
	// 投擲地点で停止しているかを返す
	return throwState_ == ThrowState::kHold;
}

void PlayerSubWeapon::SetThrowData(const PlayerSubWeaponThrowData& data) {
	// コンボ側から渡された投擲調整データを反映する
	throwData_ = data;
	throwData_.throwLifeTime = (std::max)(throwData_.throwLifeTime, 0.001f);
	throwData_.returnTime = (std::max)(throwData_.returnTime, 0.001f);
	throwData_.lerpTime = (std::max)(throwData_.lerpTime, 0.001f);
}

void PlayerSubWeapon::DrawEffect() {

}

void PlayerSubWeapon::Draw2D() {

}

void PlayerSubWeapon::UpdateIdle() {
	// 所有キャラクターがいる場合はプレイヤーの近くに待機させる
	if (character) {
		GetWorldTransform().translate_ = character->GetWorldPosition() + throwData_.idleOffset;
	}
}

void PlayerSubWeapon::UpdateThrow(float dt) {
	// 投擲時間を進める
	throwTimer_ += dt;

	// 移動タイプに応じて投擲位置を更新する
	switch (throwData_.moveType) {
	case PlayerSubWeaponThrowData::MoveType::kTeleportToTarget:
		GetWorldTransform().translate_ = throwData_.targetPosition;
		StopAtCurrentPosition();
		break;
	case PlayerSubWeaponThrowData::MoveType::kLerpToTarget:
	{
		const float rate = (std::min)(throwTimer_ / throwData_.lerpTime, 1.0f);
		GetWorldTransform().translate_ = Vector3::Lerp(throwStartPosition_, throwData_.targetPosition, rate);
		if (rate >= 1.0f) {
			StopAtCurrentPosition();
		}
		break;
	}
	case PlayerSubWeaponThrowData::MoveType::kStopOnHit:
	case PlayerSubWeaponThrowData::MoveType::kStraight:
	default:
		GetWorldTransform().translate_ += throwDirection_ * throwData_.throwSpeed * dt;
		break;
	}

	// 必要なら投擲中に回転させる
	if (throwData_.useSpin) {
		GetWorldTransform().rotate_.z += throwData_.spinSpeed * dt;
	}

	// 自動戻りが有効なら投擲時間後に回収へ移る
	if (throwData_.autoReturn && throwTimer_ >= throwData_.throwLifeTime) {
		Recall();
	}
}

void PlayerSubWeapon::UpdateHold(float dt) {
	// 停止中も演出用スピンを継続できる
	if (throwData_.useSpin) {
		GetWorldTransform().rotate_.z += throwData_.spinSpeed * dt;
	}
}

void PlayerSubWeapon::UpdateReturn(float dt) {
	// 戻り時間を進める
	throwTimer_ += dt;

	// 所有者が無ければ待機状態へ戻す
	if (!character) {
		throwState_ = ThrowState::kIdle;
		throwTimer_ = 0.0f;
		return;
	}

	// プレイヤー近くの待機位置へ補間で戻す
	const Vector3 targetPosition = character->GetWorldPosition() + throwData_.idleOffset;
	const float rate = (std::min)(throwTimer_ / throwData_.returnTime, 1.0f);
	GetWorldTransform().translate_ = Vector3::Lerp(returnStartPosition_, targetPosition, rate);
	if (throwData_.useSpin) {
		GetWorldTransform().rotate_.z += throwData_.spinSpeed * dt;
	}

	// 戻り切ったら待機状態へ戻す
	if (rate >= 1.0f) {
		throwState_ = ThrowState::kIdle;
		throwTimer_ = 0.0f;
		UpdateIdle();
	}
}
