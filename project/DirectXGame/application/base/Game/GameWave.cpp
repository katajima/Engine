#include "GameWave.h"

void WaveManager::Initialize(const std::vector<GameWave>& waves)
{
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
}

void WaveManager::EndWave(){
}
