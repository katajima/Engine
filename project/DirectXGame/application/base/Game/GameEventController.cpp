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
	characterSpawnManager_->Initialize(characterManager_, entity3DManager_->Get3DLineCommon());


	waveManager_ = std::make_unique<WaveManager>();
	waveManager_->Initialize({});

	//// スポーン情報初期化
	SpawnInfo data;
	data.Initialize("test",3,30);
	data.GetData().size_ = { 50,1,50 };
	data.GetData().spawnInterval_ = 15.0f;
	data.GetData().spawnTimer_ = 0.0f;
	characterSpawnManager_->AddCharacterSpawn(data);



	characterSpawnManager_->GetCharacterSpawn("test")->GetSpawnTransform().translate_ = { 0,0,100 };
}


void GameEventController::Update(float dt) {
	// キャラクター出現管理更新
	characterSpawnManager_->Update();


	waveManager_->Update(dt);

	bool isEndEvent = characterSpawnManager_->GetCharacterSpawn("test")->GetSpawnInfo().IsEnd();
	
	
	
	
	bool isCharaPlayerDed = !characterManager_->GetPlayer()->GetAlive();
	bool isCharaEnemyDed = characterManager_->GetCharacterCount(CharacterType::Enemy) <= 0;


	if (isCharaPlayerDed) {
		isEndEvent_ = true;
	}
	if (isEndEvent && isCharaEnemyDed) {
		isEndEvent_ = true;
	}

};

