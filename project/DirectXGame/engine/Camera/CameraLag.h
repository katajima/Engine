#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"


/// <summary>
/// CameraLagを管理・実装するクラス。
/// </summary>
class CameraLag {
public:

	Vector3 Update(float dt);

private:
	CameraLagData data;
};