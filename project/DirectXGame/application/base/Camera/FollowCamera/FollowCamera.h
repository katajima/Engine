#pragma once
#include "DirectXGame/application/base/BaseClass/Camera/BaseFollowCamera.h"

class FollowCamera : public BaseFollowCamera
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

};

