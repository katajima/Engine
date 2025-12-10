#pragma once
#include <vector>

/// <summary>
/// ゲームのウェーブ
/// </summary>
class GameWave {
public:

    int waveIndex;
   // std::vector<SpawnInfo> spawns; // このウェーブで出す敵
    float nextWaveDelay;           // 次ウェーブまでの時間


private:

};

// ウェーブ管理クラス
class WaveManager
{
public:
    void Initialize(const std::vector<GameWave>& waves);
    void Update(float dt);
    bool IsWaveActive() const;
    int  GetCurrentWave() const;

private:
    int currentWaveIndex = 0;
    float waveTimer = 0.0f;

    std::vector<GameWave> waveList;
    void StartWave(int index);
    void UpdateWave(float dt);
    void EndWave();
};