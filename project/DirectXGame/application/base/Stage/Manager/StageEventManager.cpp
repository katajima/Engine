#include "StageEventManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/BaseCharacterManeger.h"



void StageEventManager::Initialize(Entity3DManager* entity3DManager, GlobalVariables* globalVariables, BaseCharacterManager* characterManager)
{
	entity3DManager_ = entity3DManager;		// エンティティ3d
	globalVariables_ = globalVariables;		// 保存項目
	characterManager_ = characterManager;	// キャラクター管理


	// キャラクター出現管理クラス初期化
	characterSpawnManager_ = std::make_unique<CharacterSpawnManager>();
	characterSpawnManager_->Initialize(characterManager_, entity3DManager_->Get3DLineCommon());

	// スポーン情報初期化
	SpawnInfo data;
	data.Initialize("test",1,30);
	data.size_ = { 50,1,50 };
	data.spawnInterval_ = 10.0f;
	data.spawnTimer_ = 0.0f;
	characterSpawnManager_->AddCharacterSpawn(data);

	characterSpawnManager_->GetCharacterSpawn("test")->GetSpawnTransform().translate_ = { 0,0,100 };

}


void StageEventManager::Update() {
	// キャラクター出現管理更新
	characterSpawnManager_->Update();


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



void Event::Initialize(const EventData& data)
{
	data_ = data;
}

void Event::Update() {
	timer += MyGame::GameTime();


	if (data_.maxTimer <= timer) {

	}
}