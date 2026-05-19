#include "CameraCollision.h"

// 初期化
void CameraCollision::Initialize() {

}

// 更新
void CameraCollision::Update(Transform& transform, float dt) {
	if (!isCollision) return;
	
	currentPos_ = transform.translate;
	Vector3 targetPos{};
	if (target) {
		targetPos = target->GetWorldPosition();
	}
	// 地面への沈み補正
	if (currentPos_.y < 1.0f) {
		float depth = -currentPos_.y;

		float zAdjust = std::clamp(depth * depthScale_, 0.0f, maxZOffset_);

		Vector3 direction = Normalize(Subtract(targetPos, currentPos_));
		Vector3 zOffset = Multiply(direction, zAdjust);

		currentPos_ = Add(currentPos_, zOffset);
		currentPos_.y = 1.0f;
	}

	// 位置設定
	transform.translate = currentPos_;
}
