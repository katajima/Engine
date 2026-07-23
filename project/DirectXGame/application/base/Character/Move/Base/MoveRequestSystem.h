#pragma once
#include "DirectXGame/application/base/Character/Move/Base/LocomotionCoordinator.h"

/// <summary>
/// MoveRequestSystemを管理・実装するクラス。
/// </summary>
class MoveRequestSystem {
public:
	// 更新
	void Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator);

	void SetRequest(const MoveRequest& request) { requests.push_back(request); }
private:
	std::vector<MoveRequest> requests;
};
