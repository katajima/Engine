#include "GameWave.h"




// 初期化
void GameWave::Initialize(int waveIndex, float nextWaveDelay) {
	waveIndex_ = waveIndex;			// ウェーブ
	nextWaveDelay_ = nextWaveDelay;	// 次のウェーブに行く時間
}
void GameWave::AddSpawns(const SpawnInfo& spawn){
	spawns_.push_back(spawn);
}

// 更新
void GameWave::Update(float dt) {

};


void WaveManager::Initialize(const std::vector<GameWave>& waves){
	waveList = waves;
}

bool WaveManager::IsWaveActive() const{
	return false;
}

int WaveManager::GetCurrentWave() const{
	return 0;
}

void WaveManager::StartWave(int index){
}


void WaveManager::Update(float dt) {

}

void WaveManager::UpdateWave(float dt){
	waveTimer += dt;





	for (auto& wave : waveList) {
		wave.Update(dt);
	}


}

void WaveManager::EndWave(){
}
