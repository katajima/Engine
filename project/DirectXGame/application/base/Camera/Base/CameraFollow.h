#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"

// カメラ追従
class CameraFollow {
public:

	// 更新
	Vector3 Update(float dt);

private:
	// データ
	CameraFollowData data;
};