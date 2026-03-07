#pragma once
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/Move/Base/LocomotionCoordinator.h"

/// <summary>
/// 応答移動システム
/// </summary>
class ResponseMoveSystem {
public:

	// 初期化
	void Initialize();
	// 更新
	void Update(const LocomotionContext& ctx, LocomotionCoordinator& coordinator);

	void SetRequest(const MoveRequest& request) { reuest = request; }

private:
	MoveRequest reuest;

};