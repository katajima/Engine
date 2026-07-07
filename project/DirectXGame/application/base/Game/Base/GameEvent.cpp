#include "GameEvent.h"
#include <DirectXGame/application/base/Character/Spawn/CharacterSpawnManager.h>
#include <DirectXGame/application/base/Character/Base/CharacterManager.h>
#include <DirectXGame/application/base/Input/InputSystem.h>

#pragma region GameEventState

// 開始
void Game::GameEventState::Enter(Character::CharacterManager* characterManager, Character::CharacterSpawnManager* characterSpawnManager
	, InputSystem* input){
	this->characterManager = characterManager;
	this->characterSpawnManager = characterSpawnManager;
	this->input = input;

	stateTime = 0.0f;
	// 出現情報を元に出現処理
	SpawnProcess();

	if (data_.enemyDelete) {
		this->characterManager->Clear(Character::Type::Enemy);
	}

	// ウェーブ区切りでは即削除せず、敵自身の退場ステートへ移行させる
	if (data_.enemyWaveExit) {
		this->characterManager->BeginEnemyWaveExit(data_.time_.max);
	}
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

void Game::GameEventState::AddSpawns(std::vector<Character::SpawnInfo>&& spawns){
	spawns_.insert(spawns_.end(),
		std::make_move_iterator(spawns.begin()),
		std::make_move_iterator(spawns.end()));
}

float Game::GameEventState::GetCurrentTimer() const{
	return  data_.time_.max - stateTime;
}

float Game::GameEventState::RemainingTime() const {
	return data_.time_.max - stateTime;
}

// 終了したか
bool Game::GameEventState::FinishProcess() {
	switch (data_.changeType_)
	{
	case Game::GameEventChangeType::kTime:

		if (data_.time_.max < stateTime) {
			data_.time_.max = stateTime;
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
	case Game::GameEventChangeType::kInput:
		if (input->GetGameInputData().decisionTrigger) {
			return true;
		}
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


void Game::EventStateMachine::Initialize(Character::CharacterManager* characterManager, Character::CharacterSpawnManager* characterSpawnManager
	, InputSystem* input){
	
	this->characterManager = characterManager;
	this->characterSpawnManager = characterSpawnManager;
	this->input = input;
}

// ステート設定
void Game::EventStateMachine::SetState(std::shared_ptr<GameEventState> state) {
	if (currentState) currentState->Exit();	// 終了処理
	currentState = state;
	if (currentState) {
		currentState->Enter(characterManager, characterSpawnManager, input);	// 開始処理
	}
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

