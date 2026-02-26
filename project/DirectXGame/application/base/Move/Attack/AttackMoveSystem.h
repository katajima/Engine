#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/Move/Base/LocomotionCoordinator.h"


class AttackMoveSystem {
public:
	// 初期化
	void Initialize();
	// 更新(操作用)
	void Update(const LocomotionContext& ctx, LocomotionCoordinator& coordinator, Engine::WorldTransform& world, InputSystem* input);

	void SetRequest(const MoveRequest& request) { reuest = request; }

private:
	MoveRequest reuest;
};