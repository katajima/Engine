#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"


// ロックオン
/// <summary>
/// CameraLockOnを管理・実装するクラス。
/// </summary>
class CameraLockOn {
public:
	// 更新
	Vector3 Update(const Transform& transform, float dt);

	CameraLockOnData& GetData() { return data; }
	const CameraLockOnData& GetData() const { return data; }

	
private:
	CameraLockOnData data;
};
