#pragma once
#include "DirectXGame/application/base/Camera/Base/BaseFixedCamera.h"


/// <summary>
/// ミサイルを映すカメラ
/// </summary>
class UniverseCamera : public BaseFixedCamera
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input, Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, Vector3 position) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

private:
	LerpMinMax scaleLerp_;

	float strScaleZCamera = 5.5f;
	float endScaleZCamera = 1.0f;

};

