#include "ReactionMoveSystem.h"

// 初期化
void ReactionMoveSystem::Initialize() {

}
// 更新
void ReactionMoveSystem::Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {
	coordinator.Request(reuest);
	reuest.velocity = 0.0f;
}

