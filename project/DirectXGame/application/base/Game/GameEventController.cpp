#include "GameEventController.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"



void GameEventController::Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, CharacterManager* characterManager)
{
	entity3DManager_ = entity3DManager;		// エンティティ3d
	globalVariables_ = globalVariables;		// 保存項目
	characterManager_ = characterManager;	// キャラクター管理


	// キャラクター出現管理クラス初期化
	characterSpawnManager_ = std::make_unique<CharacterSpawnManager>();
	characterSpawnManager_->Initialize(characterManager_, entity3DManager_->Get3DLineCommon(),300);


	// ウェーブ管理クラス初期化
	waveManager_ = std::make_unique<WaveManager>();
	waveManager_->SetCharacterSpawnManager(characterSpawnManager_.get());
	waveManager_->SetCharacterManager(characterManager_);


	CreateSpawn(EnemyType::kNormal,"normal", 1, 2, { 0,0,0 }, { 100,1,100 }, 10.0f);
	CreateSpawn(EnemyType::kSmallRanged,"smallRanged", 1, 5, { 0,0,0 }, { 100,1,100 }, 5.0f);
	CreateSpawn(EnemyType::kSmallMelee,"smallMelee", 1, 15, { 0,0,0 }, { 100,1,100 }, 5.0f);
	CreateWave(0,10, WaveEndType::kKill,35);
	CreateSpawn(EnemyType::kNormal, "normal", 1, 3, { 0,0,0 }, { 100,1,100 }, 10.0f);
	CreateSpawn(EnemyType::kSmallRanged, "smallRanged", 1, 5, { 0,0,0 }, { 100,1,100 }, 5.0f);
	CreateSpawn(EnemyType::kSmallMelee, "smallMelee", 1, 20, { 0,0,0 }, { 100,1,100 }, 5.0f);
	CreateWave(1,10, WaveEndType::kKill,35);
	CreateSpawn(EnemyType::kNormal, "normal", 1, 3, { 0,0,0 }, { 100,1,100 }, 10.0f);
	CreateSpawn(EnemyType::kSmallRanged, "smallRanged", 1, 5, { 0,0,0 }, { 100,1,100 }, 5.0f);
	CreateSpawn(EnemyType::kSmallMelee, "smallMelee", 1, 15, { 0,0,0 }, { 100,1,100 }, 5.0f);
	CreateWave(2,10, WaveEndType::kKill,45);

	waveManager_->Initialize(gameWaves_);
}

void GameEventController::CreateWave(int waveIndex, float nextWaveDelay,WaveEndType endType, int maxEnemyCount) {
	GameWave wave;
	wave = WaveManager::CreateGameWave(waveIndex, nextWaveDelay, endType, characterManager_ ,spawnInfos_);
	wave.SetEnemyMaxCount(maxEnemyCount);
	gameWaves_.push_back(wave);
	spawnInfos_.clear();
}

void GameEventController::CreateSpawn(EnemyType type,const std::string& name, int spawnMaxCount, int spawnAmount, Vector3 translate, Vector3 size, float interval,float startDelay){
	SpawnInfo data;
	data.GetData().type_ = type;

	data.Initialize(name, spawnMaxCount, spawnAmount, translate, size, interval);
	spawnInfos_.push_back(data);
};


void GameEventController::Update(float dt) {
	// キャラクター出現管理更新
	characterSpawnManager_->Update(dt);
	// ウェーブ管理クラス
	waveManager_->Update(dt);


	curretWave_ = waveManager_->GetCurrentWave() + 1;
	time_ = waveManager_->GetCurrentWaveTime();


	bool isEndWave = waveManager_->IsEndWave();
	bool isCharaPlayerDed = !characterManager_->GetPlayer()->GetAlive();
	bool isCharaEnemyDed = characterManager_->GetCharacterCount(CharacterType::Enemy) <= 0;


	if (isCharaPlayerDed) {
		isEndEvent_ = true;
	}
	if (isEndWave) {
		isEndEvent_ = true;
	}

}

