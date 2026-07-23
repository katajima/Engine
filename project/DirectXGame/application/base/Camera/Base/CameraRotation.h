#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

// 回転制御
/// <summary>
/// CameraRotationを管理・実装するクラス。
/// </summary>
class CameraRotation {
public:

	// 初期化
	void Initialize();

	// 更新
	void Update(Transform& transform ,const InputSystem* input,float dt);

	// データ取得
	CameraRotationData& GetData() { return data_; }

private:
	CameraRotationData data_;
};