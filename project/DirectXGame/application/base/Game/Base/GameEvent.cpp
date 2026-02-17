#include "GameEvent.h"
#include <DirectXGame/application/base/Character/Spawn/CharacterSpawnManager.h>
#include <DirectXGame/application/base/Character/Base/CharacterManeger.h>

#pragma region GameEventState

// 開始
void Game::GameEventState::Enter(Character::CharacterManager* characterManager, Character::CharacterSpawnManager* characterSpawnManager){
	this->characterManager = characterManager;
	this->characterSpawnManager = characterSpawnManager;

	stateTime = 0.0f;
	// 出現情報を元に出現処理
	SpawnProcess();
}
// 終了
void Game::GameEventState::Exit() {
	stateTime = 0.0f;
	characterSpawnManager->ClearSpawn();
};

// 更新
void Game::GameEventState::Update(float dt) {
	stateTime += dt;
	// 終了条件に達しているなら終了
	if (FinishProcess()) {
		isFinish_ = true;
	}
};

std::string Game::GameEventState::GetNextStateName() const {
    // 空文字列でアクセスしているが、nextStatesのキーに""が存在しない場合は未定義動作になるため、
    // ここではnextStatesが空でなければ最初のキーを返すように修正
    if (!nextStates.empty()) {
        return nextStates.begin()->first;
    }
    return "";
}

void Game::GameEventState::AddSpawn(const Character::SpawnInfo& spawn){
	spawns_.push_back(spawn);
}

// 終了したか
bool Game::GameEventState::FinishProcess() {
	switch (data_.changeType_)
	{
	case Game::GameEventChangeType::kTime:

		if (data_.time_.max < stateTime) {
			return true;
		}
		break;
	case Game::GameEventChangeType::kKill:
		if (0 >= characterManager->GetCharacterCount(Character::Type::Enemy)) {
			return true;
		}
		break;
	case Game::GameEventChangeType::kTake:
		break;
	default:
		break;
	}
	return false;
};

void Game::GameEventState::SpawnProcess() {
	characterSpawnManager->ClearSpawn();
	for (auto& sp : spawns_) {
		sp.GetData().maxEnemyCount_ = 60;
		characterSpawnManager->AddCharacterSpawn(sp);
	}
};


#pragma endregion


#pragma region EventStateMachine


void Game::EventStateMachine::Initialize(Character::CharacterManager* characterManager, Character::CharacterSpawnManager* characterSpawnManager){
	this->characterManager = characterManager;
	this->characterSpawnManager = characterSpawnManager;
}

// ステート設定
void Game::EventStateMachine::SetState(std::shared_ptr<GameEventState> state) {
};
// 更新
void Game::EventStateMachine::Update(float dt) {
	// ステートが無いなら早期リターン
	if (!currentState) return;

	// 現在のステート更新
	currentState->Update(dt);


	// 移行条件に達して次のステートへ移行するなら
	if (currentState->IsFinish()) {
		auto next = currentState->HandleInput();
		// もし次のステートがあれば、遷移
		if (next) {
			SetState(next);
		}
	}
};

// 設定
void Game::EventStateMachine::SetRoot(std::shared_ptr<GameEventState> state) {
	rootState = state;
	if (rootState) {
		SetState(rootState);
	}
};

#pragma endregion

