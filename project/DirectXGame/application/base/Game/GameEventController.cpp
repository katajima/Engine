#include "GameEventController.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Game {

	void GameEventController::Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, 
		Character::CharacterManager* characterManager, Engine::Input* input)
	{
		this->entity3DManager = entity3DManager;		// エンティティ3d
		this->globalVariables = globalVariables;		// 保存項目
		this->characterManager = characterManager;	// キャラクター管理


		// キャラクター出現管理クラス初期化
		characterSpawnManager_ = std::make_unique<Character::CharacterSpawnManager>();
		characterSpawnManager_->Initialize(characterManager, entity3DManager->Get3DLineCommon(), 300);

		// イベントステート
		eventStateMachine_ = std::make_unique<Game::EventStateMachine>();
		eventStateMachine_->Initialize(characterManager, characterSpawnManager_.get(),input);


		Game::GameEventData data;
		data.eventType_ = GameEventType::kBreakTime;
		data.time_.max = 5.0f;
		CreateGameEvent("start", data);

		CreateSpawn(Character::EnemyType::kMediumMelee,"normal", 1, 2, { 0,1,500 }, { 10,1,10 }, 10.0f);
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged", 100, 1, { 0,1,500 }, { 10,1,10 }, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged2", 100, 1, { 500,1,500 }, { 10,1,10 }, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee", 100, 3, { 0,1,500 }, { 10,1,10 }, 0.75f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee2", 100, 3, { -500,1,500 }, { 10,1,10 }, 0.75f);

		data.eventType_ = GameEventType::kBattle;
		data.battleWaveIndex_ = 1;
		data.time_.max = 30.0f;

		CreateGameEvent("battle01", data);

		CreateSpawn(Character::EnemyType::kMediumMelee, "normal", 1, 2, { 0,1,500 }, { 10,1,10 }, 10.0f);
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged", 100, 1, { 0,1,-400 }, { 10,1,10 }, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee", 100, 3, { 0,0,-400 }, { 10,1,10 }, 0.75f);

		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged2", 100, 1, { 500,1,0 }, { 10,1,10 }, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee2", 100, 3, { -500,1,0 }, { 10,1,10 }, 0.75f);


		data.eventType_ = GameEventType::kBattle;
		data.battleWaveIndex_ = 2;
		data.time_.max = 40.0f;

		CreateGameEvent("battle02", data);

		data.changeType_ = GameEventChangeType::kTime;
		data.eventType_ = GameEventType::kEnd;
		data.time_.max = 3.0f;
		data.enemyDelete = true;

		CreateGameEvent("result", data);


		/*data.changeType_ = GameEventChangeType::kInput;
		data.eventType_ = GameEventType::kEnd;
		data.time_.max = 10.0f;

		CreateGameEvent("end", data);*/

		ConnectNode("start","","battle01");
		ConnectNode("battle01", "", "battle02");
		ConnectNode("battle02", "", "result");
		//ConnectNode("result", "", "end");

		eventStateMachine_->SetRoot(GetNodeState("start"));
	}


	void GameEventController::CreateSpawn(Character::EnemyType type, const std::string& name, int spawnMaxCount, int spawnAmount, Vector3 translate, Vector3 size, float interval, float startDelay) {
		Character::SpawnInfo data;
		data.GetData().type_ = type;

		data.Initialize(name, spawnMaxCount, spawnAmount, translate, size, interval);
		spawnInfos_.push_back(data);
	}

#pragma region  Event

	void GameEventController::AddNode(const std::string& nodeName, const Game::GameEventData& data)
	{
		// 既に存在する場合は追加しない
		if (eventStateNodes_.find(nodeName) != eventStateNodes_.end()) {
			return;
		}
		// ノード生成
		std::shared_ptr<Game::GameEventState> node = std::make_shared<Game::GameEventState>();
		eventStateNodes_[nodeName] = node;				// ノード追加

		// 出現情報追加
		eventStateNodes_[nodeName]->AddSpawns(std::move(spawnInfos_));
		spawnInfos_.clear();
		eventStateNodes_[nodeName]->SetData(data);			// データ設定
		eventStateNodes_[nodeName]->SetName(nodeName);	// ノード名設定
	}


	void GameEventController::ConnectNode(const std::string& from, const std::string& name, const std::string& to)
	{
		auto itFrom = eventStateNodes_.find(from);
		auto itTo = eventStateNodes_.find(to);
		if (itFrom != eventStateNodes_.end()) {
			if (itTo != eventStateNodes_.end())
				itFrom->second->SetNextState(itTo->second);
		}
	}


	void GameEventController::CreateGameEvent(const std::string& nodeName, const Game::GameEventData& data) {
		AddNode(nodeName, data);
	};

#pragma endregion // イベント系



	void GameEventController::Update(float dt) {
		// キャラクター出現管理更新
		characterSpawnManager_->Update(dt);

		// イベントステートマシン更新
		eventStateMachine_->Update(dt);


		curretWave_ = eventStateMachine_->GetCurrentState()->GetData().battleWaveIndex_;
		time_ = eventStateMachine_->GetCurrentState()->GetCurrentTimer();

		bool isCharaPlayerDed = !characterManager->GetPlayer()->GetAlive();
		bool isCharaEnemyDed = characterManager->GetCharacterCount(Character::Type::Enemy) <= 0;

		bool isEndState = eventStateMachine_->IsEventFinished();
		bool isFinish = eventStateMachine_->GetCurrentState()->IsFinish();
		if (isCharaPlayerDed) {
			isEndEvent_ = true;
		}

		// 最後のステートでなおかつ終了状態なら
		if (isEndState && isFinish) {
			isEndEvent_ = true;
		}

	}

}