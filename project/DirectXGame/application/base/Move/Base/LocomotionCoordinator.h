#pragma once
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/Move/Move/MoveRequest.h"
#include "DirectXGame/application/base/Move/Jump/JumpRequest.h"
#include "DirectXGame/application/base/Move/Dash/DashRequest.h"
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

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);
private:


private:
	InputSystem* inputSystem = nullptr;

};