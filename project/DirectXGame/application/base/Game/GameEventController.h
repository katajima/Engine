#pragma once
#include <DirectXGame/application/base/Character/Spawn/CharacterSpawnManager.h>
#include "GameWave.h"

// 前方宣言
namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
	class GlobalVariables;
}

/// <summary>
/// ゲームのイベントを管理するクラス
/// </summary>
class GameEventController {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables,  BaseCharacterManager* characterManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() {};

	// イベント終了通知
	bool IsEndEvent() const { return isEndEvent_; };

	// 現在のウェーブ
	int GetCurrentWave() const { return curretWave_; }
	// 現在の時間
	float GetCurrentWaveTime() const { return time_; }
	// ウェーブ生成
	void CreateWave(int waveIndex, float nextWaveDelay);
	// スポーン生成
	void CreateSpawn(EnemyType type, const std::string& name, int spawnMaxCount, int spawnAmount, Vector3 translate, Vector3 size = { 10,1,10 }, float interval = 1.0f);



private:

	// キャラクター出現管理クラス
	std::unique_ptr<CharacterSpawnManager> characterSpawnManager_ = nullptr;
	// ウェーブ管理クラス
	std::unique_ptr<WaveManager> waveManager_ = nullptr;
	// ウェーブ情報
	std::vector<GameWave> gameWaves_;
	// スポーン情報
	std::vector<SpawnInfo> spawnInfos_;
	bool isEndEvent_ = false;	// イベント終了フラグ
	int curretWave_ = 0;		// ウェーブ
	float time_ = 0.0f;			// 時間
private:
	Engine::Entity3DManager* entity3DManager_ = nullptr;
	Engine::GlobalVariables* globalVariables_ = nullptr;
	BaseCharacterManager* characterManager_ = nullptr;
};
