#include "CameraController.h"
#include <algorithm>
#include <cmath>

void CameraController::Initialize(Engine::Camera* camera, const InputSystem* input) {
	this->camera = camera;
	this->input = input;

	// 追従
	follow = std::make_unique<CameraFollow>();
	follow->Initialize();
	// 回転
	rotation = std::make_unique<CameraRotation>();
	rotation->Initialize();
	// ロックオン
	lockOn = std::make_unique<CameraLockOn>();
	// 判定
	collision = std::make_unique<CameraCollision>();
	// シェイク
	shake = std::make_unique<CameraShake>();
	shake->Initialize(camera);
	// ズーム
	zoom = std::make_unique<CameraZoom>();
	zoom->SetDefultZ(camera->GetTransform().scale.z);

}


void CameraController::Update(float dt) {
	// カメラトランスフォーム取得
	Transform transform = camera->GetTransform();

	// 追従対象の移動速度を、先読みや速度ズーム用に推定する
	UpdateTargetMotion(dt);
	
	// ロックオンしているなら
	if (lockOn->GetData().isLockOn) {
		transform.rotate = lockOn->Update(transform, dt);
		if (lockOn->GetData().isLockOnRotate) {
			rotation->GetData().pitch = transform.rotate.x;
			rotation->GetData().yaw = transform.rotate.y;
		}
	}
	else { // していないなら回転処理を優先
		// 回転処理
		rotation->Update(transform,input,dt);
	}
	// アクション用の先読み、速度ズーム、注視点オフセットを更新する
	UpdateActionTimers(dt);
	UpdateFollowAssist(dt);
	UpdateSpeedZoom(dt);
	// 追従
	follow->Update(transform,dt);
	// ズーム
	zoom->Update(transform.scale.z,dt);
	// シェイク
	shake->SetDefultPos(transform.translate);
	shake->Update(transform.translate,dt);
	// 判定
	collision->Update(transform, dt);
	// カメラ位置決定
	camera->SetTransform(transform);
};

void CameraController::SetTarget(const Engine::WorldTransform* target) {
	this->target = target;
	follow->SetTarget(target);
	collision->SetTarget(target);
	hasPreviousTargetPosition_ = false;
};

void CameraController::SetLockOnTarget(const Engine::WorldTransform* target) {
	this->lockOnTarget = target;
	lockOn->GetData().target = target;
	lockOn->GetData().isLockOn = target != nullptr;
}

void CameraController::SetLookAheadData(const CameraLookAheadData& data) {
	// 通常時に使う先読み設定を保存する
	lookAheadData_ = data;
}

void CameraController::SetSpeedZoomData(const CameraSpeedZoomData& data) {
	// 通常時に使う速度ズーム設定を保存する
	speedZoomData_ = data;
}

void CameraController::RequestLookAhead(const CameraLookAheadData& data, float duration) {
	// 攻撃演出などで一時的に通常設定を上書きする
	actionLookAheadData_ = data;
	actionLookAheadTimer_ = duration;
	isActionLookAheadActive_ = true;
}

void CameraController::RequestSpeedZoom(const CameraSpeedZoomData& data, float duration) {
	// 攻撃演出などで一時的に速度ズームを上書きする
	actionSpeedZoomData_ = data;
	actionSpeedZoomTimer_ = duration;
	isActionSpeedZoomActive_ = true;
}

void CameraController::RequestActionTargetOffset(const CameraActionOffsetData& data) {
	// 攻撃演出などで追従中心を一時的にずらす
	actionOffsetData_ = data;
	actionOffsetTimer_ = data.duration;
	isActionOffsetActive_ = true;
}

void CameraController::ClearActionAssist() {
	// 攻撃終了時に一時カメラ演出を解除する
	isActionLookAheadActive_ = false;
	isActionSpeedZoomActive_ = false;
	isActionOffsetActive_ = false;
	actionLookAheadTimer_ = 0.0f;
	actionSpeedZoomTimer_ = 0.0f;
	actionOffsetTimer_ = 0.0f;
}

void CameraController::UpdateTargetMotion(float dt) {
	// 追従対象が無い、または時間が進んでいないなら速度を0にする
	if (!target || dt <= 0.0f) {
		targetVelocity_ = {};
		hasPreviousTargetPosition_ = false;
		return;
	}

	// ワールド座標の差分から疑似速度を求める
	const Vector3 currentPosition = target->GetWorldPosition();
	if (hasPreviousTargetPosition_) {
		targetVelocity_ = (currentPosition - previousTargetPosition_) / dt;
	}
	else {
		targetVelocity_ = {};
		hasPreviousTargetPosition_ = true;
	}
	previousTargetPosition_ = currentPosition;
}

void CameraController::UpdateFollowAssist(float dt) {
	// 一時設定が有効ならそちらを優先し、無ければ通常設定を使う
	const CameraLookAheadData& lookAheadData =
		isActionLookAheadActive_ ? actionLookAheadData_ : lookAheadData_;

	// 速度方向の先読み位置を滑らかに補間する
	const Vector3 targetLookAheadOffset = CalculateLookAheadOffset(lookAheadData);
	const float lookAheadT = std::clamp(lookAheadData.smoothSpeed * dt, 0.0f, 1.0f);
	currentLookAheadOffset_ = Lerp(currentLookAheadOffset_, targetLookAheadOffset, lookAheadT);

	// 攻撃中の注視点オフセットを滑らかに出し入れする
	const Vector3 targetActionOffset = isActionOffsetActive_ ? actionOffsetData_.targetOffset : Vector3{};
	const float actionOffsetT = std::clamp(actionOffsetData_.blendSpeed * dt, 0.0f, 1.0f);
	currentActionTargetOffset_ = Lerp(currentActionTargetOffset_, targetActionOffset, actionOffsetT);

	// 追従カメラへ、先読みと攻撃オフセットを合成した注視点を渡す
	follow->SetTargetOffset(currentLookAheadOffset_ + currentActionTargetOffset_);
}

void CameraController::UpdateSpeedZoom(float dt) {
	// 一時設定が有効ならそちらを優先し、無ければ通常設定を使う
	const CameraSpeedZoomData& data =
		isActionSpeedZoomActive_ ? actionSpeedZoomData_ : speedZoomData_;
	const Vector3 defaultOffset = follow->GetDefultOffsetPos();

	// ターゲット速度から、後ろへ引く距離を計算して補間する
	float targetOffsetZ = 0.0f;
	if (data.enable) {
		const float speedRate = CalculateSpeedRate(data.minSpeed, data.maxSpeed);
		targetOffsetZ = Lerp(data.nearOffsetZ, data.farOffsetZ, speedRate);
	}
	const float zoomT = std::clamp(data.smoothSpeed * dt, 0.0f, 1.0f);
	currentSpeedZoomOffsetZ_ = Lerp(currentSpeedZoomOffsetZ_, targetOffsetZ, zoomT);

	// 通常の追従距離に速度ズーム分を足す
	Vector3 offset = follow->GetOffsetPos();
	offset.z = defaultOffset.z + currentSpeedZoomOffsetZ_;
	follow->SetOffsetPos(offset);
}

void CameraController::UpdateActionTimers(float dt) {
	// 0以下の時間は、外部から解除されるまで継続する設定として扱う
	if (isActionLookAheadActive_ && actionLookAheadTimer_ > 0.0f) {
		actionLookAheadTimer_ -= dt;
		if (actionLookAheadTimer_ <= 0.0f) {
			isActionLookAheadActive_ = false;
		}
	}
	if (isActionSpeedZoomActive_ && actionSpeedZoomTimer_ > 0.0f) {
		actionSpeedZoomTimer_ -= dt;
		if (actionSpeedZoomTimer_ <= 0.0f) {
			isActionSpeedZoomActive_ = false;
		}
	}
	if (isActionOffsetActive_ && actionOffsetTimer_ > 0.0f) {
		actionOffsetTimer_ -= dt;
		if (actionOffsetTimer_ <= 0.0f) {
			isActionOffsetActive_ = false;
		}
	}
}

float CameraController::CalculateSpeedRate(float minSpeed, float maxSpeed) const {
	// 速度レンジが不正なら急な挙動を避けるため0にする
	if (maxSpeed <= minSpeed) {
		return 0.0f;
	}
	const float speed = Length(targetVelocity_);
	return std::clamp((speed - minSpeed) / (maxSpeed - minSpeed), 0.0f, 1.0f);
}

Vector3 CameraController::CalculateLookAheadOffset(const CameraLookAheadData& data) const {
	// 無効時や速度がほぼ無い時は先読みしない
	if (!data.enable || targetVelocity_.LengthSq() < 1e-6f) {
		return {};
	}

	// 速度方向に、速度割合に応じた距離だけ注視点を先へ送る
	const float speedRate = CalculateSpeedRate(data.minSpeed, data.maxSpeed);
	const Vector3 direction = Normalize(targetVelocity_);
	return direction * (data.distance * speedRate);
}
