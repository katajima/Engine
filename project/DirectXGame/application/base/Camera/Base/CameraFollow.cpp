#include "CameraFollow.h"
#include <algorithm>
#include "CameraCollision.h"

void CameraFollow::Initialize() {
	data_.followSpeed = 40.0f;
	data_.useSmooth = true;
	data_.offset.y = 4.0f;
	data_.offset.z =  -20.5;
	// デフォルト位置設定
	defultOffst = data_.offset;
	// 現在位置設定
	currentPos_ = {};
	//
	isInitializedPosition_ = false;
}


void CameraFollow::Update(Transform& transform,float dt) {
	// 追従対象がいなければ何もしない
	if (!target) {
		return;
	}
	// 回転適用 → ローカルオフセットをワールド空間へ
	// カメラ回転に応じて後方・上方オフセットを回す
	Matrix4x4 rotY = MakeRotateYMatrix(transform.rotate.y);
	Matrix4x4 rotX = MakeRotateXMatrix(transform.rotate.x);

	// Yaw → Pitch の順で適用
	Matrix4x4 rotateMatrix = rotX * rotY;

	Vector3 rotatedOffset = TransformNormal(data_.offset, rotateMatrix);

	// 追従先の基準位置に、演出用の注視点オフセットを加える
	Vector3 targetPosition = Add(target->GetWorldPosition(), data_.targetOffset);

	// 目標カメラ位置
	Vector3 desiredCameraPos = Add(targetPosition, rotatedOffset);


	// 初回は補間せず即座に合わせる
	if (!isInitializedPosition_) {
		currentPos_ = desiredCameraPos;
		isInitializedPosition_ = true;
	}
	else if (data_.useSmooth) {
		const float t = std::clamp(data_.followSpeed * dt, 0.0f, 1.0f);
		currentPos_ = Lerp(currentPos_, desiredCameraPos, t);
	}
	else {
		currentPos_ = desiredCameraPos;
	}

	// 最終結果を transform に反映
	transform.translate = currentPos_;
}

