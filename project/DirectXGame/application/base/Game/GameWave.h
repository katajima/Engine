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

    // 次のウェーブまでの時間
    float GetNextWaveDelay() const { return nextWaveDelay_; };

    // 出現情報
    std::vector<SpawnInfo> GetSpawnInfo() { return spawns_; }
private:
    int waveIndex_;                     // ウェーブ
    std::vector<SpawnInfo> spawns_;     // 出現情報
    float nextWaveDelay_ = 10.0f;       // 次ウェーブまでの時間
   WaveEndType endType = WaveEndType::kTime;   // 終了方法
};

// 前方宣言
class CharacterSpawnManager;

// ウェーブ管理クラス
class WaveManager
{
public:
    
    void Initialize(const std::vector<GameWave>& waves);
    void Update(float dt);
    void SetCharacterSpawnManager(CharacterSpawnManager* spawnManager) {this->spawnManager = spawnManager;}
  
    // 全てのウェーブ終了
    bool IsEndWave() const { return isEndWave; }
    // 現在のウェーブが最終ウェーブか
    bool IsCurrentEndWave() const { return isCurrentEndWave; }
    // 現在のウェーブ
    int GetCurrentWave() const { return currentWaveIndex; }
    //
    float GetCurrentWaveTime() const;

    /// <summary>
    /// ゲームウェーブ生成
    /// </summary>
    /// <returns></returns>
    static GameWave CreateGameWave(int waveIndex, float nextWaveDelay,const std::vector<SpawnInfo>& spawns);

   
private:

    CharacterSpawnManager* spawnManager = nullptr;
    int currentWaveIndex = 0;
    float waveTimer = 0.0f;
    bool isEndWave = false;
   
    bool isCurrentEndWave = false;
   
    std::vector<GameWave> waveList;
};