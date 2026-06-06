#include "PlayerSubWeapon.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <algorithm>

void PlayerSubWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
	this->inputSystem = inputSystem;						// インプット
	this->entityManager = entityManager; // エンティティ3d
	this->globalVariables = globalVariables;				// グローバル変数
	this->camera = camera;								// カメラ

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entityManager, globalVariables, "PlayerSubWeapon", "Sword.obj", false, false, this);
	objectComponent_->SetSRT({ 0.55f,0.55f,0.55f }, { 0.0f,0.0f,0.0f }, position);	// SRT設定

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
	GetWorldTransform().translate_ = startPosition;
}

bool PlayerSubWeapon::IsThrowing() const {
	// 投擲または戻りの間はサブ武器攻撃中として扱う
	return throwState_ != ThrowState::kIdle;
}

void PlayerSubWeapon::DrawEffect() {

}

void PlayerSubWeapon::Draw2D() {

}

void PlayerSubWeapon::UpdateIdle() {
	// 所有キャラクターがいる場合はプレイヤーの近くに待機させる
	if (character) {
		GetWorldTransform().translate_ = character->GetWorldPosition() + idleOffset_;
	}
}

void PlayerSubWeapon::UpdateThrow(float dt) {
	// 投擲時間を進める
	throwTimer_ += dt;

	// 前方へ飛ばしながら軽く回転させる
	GetWorldTransform().translate_ += throwDirection_ * throwSpeed_ * dt;
	GetWorldTransform().rotate_.z += spinSpeed_ * dt;

	// 投擲時間が終わったら戻り状態へ移行する
	if (throwTimer_ >= throwLifeTime_) {
		throwTimer_ = 0.0f;
		returnStartPosition_ = GetWorldTransform().translate_;
		throwState_ = ThrowState::kReturn;
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
	const Vector3 targetPosition = character->GetWorldPosition() + idleOffset_;
	const float rate = (std::min)(throwTimer_ / returnTime_, 1.0f);
	GetWorldTransform().translate_ = Vector3::Lerp(returnStartPosition_, targetPosition, rate);
	GetWorldTransform().rotate_.z += spinSpeed_ * dt;

	// 戻り切ったら待機状態へ戻す
	if (rate >= 1.0f) {
		throwState_ = ThrowState::kIdle;
		throwTimer_ = 0.0f;
		UpdateIdle();
	}
}
