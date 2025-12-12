#pragma once
#include <vector>

#include <DirectXGame/application/base/Character/Spawn/CharacterSpawn.h>


enum class WaveEndType {
    kTime,      // 時間で
    kKill,      // 全部倒したら
};


/// <summary>
/// ゲームのウェーブ
/// </summary>
class GameWave {
public:

    // 初期化
    void Initialize(int waveIndex,float nextWaveDelay);
    // スポーン追加
    void AddSpawns(const SpawnInfo& spawn);

    // 更新
    void Update(float dt);



private:
    int waveIndex_;                     // ウェーブ
    std::vector<SpawnInfo> spawns_;     // 出現情報
    float nextWaveDelay_;               // 次ウェーブまでの時間
    WaveEndType endType = WaveEndType::kTime;   // 終了方法
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