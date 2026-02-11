#pragma once
#include <vector>

#include <DirectXGame/application/base/Character/Spawn/CharacterSpawn.h>


enum class WaveEndType {
	kTime,      // 時間で
	kKill,      // 全部倒したら
};
namespace Character {
	// 前方宣言
	class CharacterSpawnManager;
	class CharacterManager;
}
/// <summary>
/// ゲームのウェーブ
/// </summary>
class GameWave {
public:

	// 初期化
	void Initialize(int waveIndex, float nextWaveDelay, WaveEndType endType);
	// スポーン追加
	void AddSpawns(const Character::SpawnInfo& spawn);

	void SetCharacterManager(Character::CharacterManager* characterManager) { this->characterManager = characterManager; }


	// 更新
	void Update(float dt);

	// 次のウェーブまでの時間
	float GetNextWaveDelay() const { return nextWaveDelay_; };

	// ウェーブが終了したか
	bool IsEndWave() const { return isEndWave_; };


	//
	int GetEnemymaxCount() const { return maxEnemyCount_; }
	//
	void SetEnemyMaxCount(int count) { maxEnemyCount_ = count; }

	// 出現情報
	std::vector<Character::SpawnInfo> GetSpawnInfo() { return spawns_; }
private:
	int waveIndex_;                     // ウェーブ
	std::vector<Character::SpawnInfo> spawns_;     // 出現情報
	float nextWaveDelay_ = 10.0f;       // 次ウェーブまでの時間
	float  waveTimer = 0.0f;            // 時間計測
	int maxEnemyCount_ = 30;			// 最大敵出現量
	bool isEndWave_ = false;            // ウェーブ終了フラグ
	Character::CharacterManager* characterManager = nullptr;

	WaveEndType endType_ = WaveEndType::kTime;   // 終了方法
};

// ウェーブ管理クラス
class WaveManager
{
public:

	void Initialize(const std::vector<GameWave>& waves);
	void Update(float dt);
	void SetCharacterSpawnManager(Character::CharacterSpawnManager* spawnManager) { this->spawnManager = spawnManager; }

	void SetCharacterManager(Character::CharacterManager* characterManager) { this->characterManager = characterManager; }

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
	static GameWave CreateGameWave(int waveIndex, float nextWaveDelay, WaveEndType endType, Character::CharacterManager* characterManager, const std::vector<Character::SpawnInfo>& spawns);


private:
	Character::CharacterManager* characterManager = nullptr;
	Character::CharacterSpawnManager* spawnManager = nullptr;
	int currentWaveIndex = 0;
	float waveTimer = 0.0f;
	bool isEndWave = false;

	bool isCurrentEndWave = false;

	std::vector<GameWave> waveList;
};
