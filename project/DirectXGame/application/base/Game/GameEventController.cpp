#include "GameEventController.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/BaseCharacterManeger.h"



void GameEventController::Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, BaseCharacterManager* characterManager)
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


	CreateSpawn(EnemyType::kNormal,"test", 1, 20, { 0,0,100 }, { 50,1,25 }, 15.0f);
	CreateSpawn(EnemyType::kNormal,"test1", 3, 10, { 100,0,-100 }, { 25,1,25 }, 5.0f);
	CreateSpawn(EnemyType::kNormal,"test2", 3, 10, { -100,0,-100 }, { 25,1,25 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test3", 10, 3, { 100,0,-100 }, { 20,1,20 }, 3.0f);
	CreateSpawn(EnemyType::kNormal, "test4", 10, 3, { -100,0,-100 }, { 20,1,20 }, 3.0f);
	CreateWave(0,40);
	CreateSpawn(EnemyType::kNormal,"test", 1, 20, { 0,0,100 }, { 50,1,20 }, 15.0f);
	CreateSpawn(EnemyType::kNormal, "test1", 3, 10, { 30,0,-100 }, { 20,1,20 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test2", 3, 10, { -30,0,-100 }, { 20,1,20 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test3", 10, 4, { 30,0,-100 }, { 20,1,20 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test4", 10, 4, { -30,0,-100 }, { 20,1,20 }, 5.0f);
	CreateWave(1,40);
	CreateSpawn(EnemyType::kNormal, "test", 1, 30, { 0,0,50 }, { 50,1,25 }, 15.0f);
	CreateSpawn(EnemyType::kNormal, "test1", 3, 20, { 100,0,50 }, { 25,1,25 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test2", 3, 20, { -100,0,-50 }, { 25,1,25 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test3", 10, 7, { 0,0,50 }, { 50,1,25 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test4", 10, 7, { 100,0,50 }, { 25,1,25 }, 5.0f);
	CreateSpawn(EnemyType::kNormal, "test5", 10, 7, { -100,0,-50 }, { 25,1,25 }, 5.0f);
	CreateWave(2,50);

	waveManager_->Initialize(gameWaves_);
}

void GameEventController::CreateWave(int waveIndex, float nextWaveDelay) {
	GameWave wave;
	wave = WaveManager::CreateGameWave(waveIndex, nextWaveDelay,spawnInfos_);
	gameWaves_.push_back(wave);
	spawnInfos_.clear();
}

void GameEventController::CreateSpawn(EnemyType type,const std::string& name, int spawnMaxCount, int spawnAmount, Vector3 translate, Vector3 size, float interval){
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

