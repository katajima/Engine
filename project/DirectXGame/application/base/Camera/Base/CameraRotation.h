#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"

// 回転制御
/// <summary>
/// CameraRotationを管理・実装するクラス。
/// </summary>
class CameraRotation {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Transform& transform ,const InputSystem* input,float dt);

	/// <summary>
	/// データ取得
	/// </summary>
	CameraRotationData& GetData() { return data_; }

private:
	CameraRotationData data_;
};