#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

// 回転制御
class CameraRotation {
public:

	// 初期化
	void Initialize();

	// 更新
	Vector3 Update(const InputSystem* input,float dt);

private:
	CameraRotationData data_;
};