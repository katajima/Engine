#pragma once
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/Move/Move/MoveRequest.h"

/// <summary>
/// 移動関連の調停役クラス
/// </summary>
class LocomotionCoordinator {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="inputSystem"></param>
	void Initialize(InputSystem* inputSystem);

private:
	InputSystem* inputSystem = nullptr;

};