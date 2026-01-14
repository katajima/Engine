#include "GameWave.h"
#include <DirectXGame/application/base/Character/Spawn/CharacterSpawnManager.h>
#include <DirectXGame/application/base/Character/Base/CharacterManeger.h>


#pragma region GameWave

// 初期化
void GameWave::Initialize(int waveIndex, float nextWaveDelay, WaveEndType endType) {
	waveIndex_ = waveIndex;			// ウェーブ
	nextWaveDelay_ = nextWaveDelay;	// 次のウェーブに行く時間
	endType_ = endType;			// 終了方法
}
void GameWave::AddSpawns(const SpawnInfo& spawn) {
	spawns_.push_back(spawn);
}

// 更新
void GameWave::Update(float dt) {
	waveTimer += dt;

	if (endType_ == WaveEndType::kTime) {
		if (waveTimer >= GetNextWaveDelay()) {
			isEndWave_ = true;
		}
	} else {
		if (0 >= characterManager->GetCharacterCount(CharacterType::Enemy) && waveTimer >= GetNextWaveDelay()) {
			isEndWave_ = true;
		}
	}
};

#pragma endregion // ゲームウェーブ


#pragma region WaveManager

void WaveManager::Initialize(const std::vector<GameWave>& waves){
	waveList = waves;

	// 出現
	for (auto& sp : waveList[currentWaveIndex].GetSpawnInfo()) {
		spawnManager->AddCharacterSpawn(sp);
	}
}

void WaveManager::Update(float dt){
	
	// サイズ
	if (waveList.size() <= currentWaveIndex) {
		isEndWave = true;
		return;
	}

	if (waveList.size() <= currentWaveIndex - 1) {
		isCurrentEndWave = true;
	}

	
	waveTimer += dt;
	// 更新現在のウェーブ更新
	
	waveList[currentWaveIndex].Update(dt);
	// 次のウェーブまでの時間
	if (waveList[currentWaveIndex].IsEndWave()) {
		waveTimer = 0.0f;

		spawnManager->ClearSpawn();
		currentWaveIndex++;

		// サイズ
		if (waveList.size() <= currentWaveIndex) {
			isEndWave = true;
			return;
		}

		for (auto& sp : waveList[currentWaveIndex].GetSpawnInfo()) {
			sp.GetData().maxEnemyCount_ = waveList[currentWaveIndex].GetEnemymaxCount();
			spawnManager->AddCharacterSpawn(sp);
		}
	}
}

float WaveManager::GetCurrentWaveTime() const{

	if (waveList.size() <= currentWaveIndex) {
		return  0;
	}
	else {
		return  waveList[currentWaveIndex].GetNextWaveDelay() - waveTimer;
	}
}

GameWave WaveManager::CreateGameWave(int waveIndex, float nextWaveDelay, WaveEndType endType,CharacterManager* characterManager, const std::vector<SpawnInfo>& spawns){

	GameWave wave;
	wave.Initialize(waveIndex,nextWaveDelay, endType);
	wave.SetCharacterManager(characterManager);
	for (auto& sp : spawns) {
		wave.AddSpawns(sp);
	}
	return wave;
}

#pragma endregion //　ウェーブ管理クラス