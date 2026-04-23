#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"

class CameraShake {
public:

	// 更新
	Vector3 Update(float dt);

private:
	CameraShakeData data;
};