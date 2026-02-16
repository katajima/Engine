#include "GameEvent.h"


// 初期化
void Game::GameEvent::Initialize() {
};


// 更新
void Game::GameEvent::Update(float dt) {
};

// 終了したか
bool Game::GameEvent::IsFinish() {
	switch (changeType_)
	{
	case Game::GameEventChangeType::kTime:

		if (time_.max < time_.time) {
			return true;
		}
		break;
	case Game::GameEventChangeType::kKill:
		break;
	case Game::GameEventChangeType::kTake:
		break;
	default:
		break;
	}
	return false;
};