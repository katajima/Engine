#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"


class CameraLag {
public:

	Vector3 Update(float dt);

private:
	CameraLagData data;
};