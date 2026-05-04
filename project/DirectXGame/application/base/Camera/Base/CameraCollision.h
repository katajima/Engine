#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"

// 
class CameraCollision {
public:

	// 初期化
	void Initialize();

	// 更新
	void Update(Transform& transform, float dt);
	// ターゲット設定
	void SetTraget(const Engine::WorldTransform* target) { this->target = target; };

private:
	CameraCollisionData data_;
	bool isCollision = true;
	// 現在のカメラ位置
	Vector3 currentPos_{};

	float maxZOffset_ = 30.0f;
	float depthScale_ = 2.0f;
private:
	// ターゲット
	const Engine::WorldTransform* target = nullptr;
};