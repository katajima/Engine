#include "GameEventController.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

namespace Game {

	void GameEventController::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
		CameraManager* cameraManager, Character::CharacterManager* characterManager, InputSystem* input)
	{
		this->entityManager = entityManager;		// エンティティ3d
		this->globalVariables = globalVariables;		// 保存項目
		this->characterManager = characterManager;	// キャラクター管理
		this->cameraManager = cameraManager;		// カメラ管理

		// キャラクター出現管理クラス初期化
		characterSpawnManager_ = std::make_unique<Character::CharacterSpawnManager>();
		characterSpawnManager_->Initialize(characterManager, entityManager->Get3DLineCommon(), 400);

		// イベントステート初期化
		eventStateMachine_ = std::make_unique<Game::EventStateMachine>();
		eventStateMachine_->Initialize(characterManager, characterSpawnManager_.get(), input);

		// ゲームスタートUI初期化
		gameStartUI_ = std::make_unique<GameStartUI>();
		gameStartUI_->Initialize(nullptr, entityManager, globalVariables);

		playerDieTime = 0.0f;

		Game::GameEventData data;
		data.eventType_ = GameEventType::kStart;
		data.time_.max = 2.5f;
		data.isMove = false;
		CreateGameEvent("start", data);

		data.isMove = true;
		data.eventType_ = GameEventType::kBreakTime;
		data.time_.max = 1.5f;
		CreateGameEvent("breakTime", data);

		Vector3 popPos = { 0,1,100 };
		Vector3 popPos2 = { 100,1,100 };
		Vector3 popPos3 = { 0,1,100 };
		Vector3 popPos4 = { -100,1,100 };
		Vector3 popPos5 = { 0,1,-100 };
		Vector3 popPos6 = { 100,1,0 };
		Vector3 popPos7 = { -100,1,0 };

		Vector3 size = {50,1,50};

		CreateSpawn(Character::EnemyType::kMediumMelee, "normal", 1, 1, popPos, size, 10.0f);
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged", 100, 1, popPos, size, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged2", 100, 1, popPos2, size, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee", 100, 4, popPos, size, 0.75f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee2", 100, 4, popPos4, size, 0.75f);
		CreateSpawn(Character::EnemyType::kMediumMelee, "normal", 1, 3, { 0,1,500 }, { 10,1,10 }, 10.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee", 1, 3, { 0,1,-500 }, { 10,1,10 }, 0.75f);
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged", 1, 1, { 500,1,500 }, { 10,1,10 }, 4.0f);

		data.eventType_ = GameEventType::kBattle;
		data.battleWaveIndex_ = 1;
		data.time_.max = 30.0f;

		CreateGameEvent("battle01", data);

		CreateSpawn(Character::EnemyType::kMediumMelee, "normal", 1, 2, popPos, size, 10.0f);
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged", 100, 1, popPos5, size, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee", 100, 5, popPos5, size, 0.75f);
		
		CreateSpawn(Character::EnemyType::kSmallRanged, "smallRanged2", 100, 1, popPos6, size, 4.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "smallMelee2", 100, 5, popPos7, size, 0.75f);


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

		ConnectNode("start", "", "breakTime");
		ConnectNode("breakTime", "", "battle01");
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

	void GameEventController::AddNode(const std::string& nodeName, const Game::GameEventData& data) {
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


	void GameEventController::ConnectNode(const std::string& from, const std::string& name, const std::string& to) {
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

		// ゲーム開始UI
		gameStartUI_->SetNum(time_);
		gameStartUI_->Update(dt);

		if (eventStateMachine_->GetCurrentState()->GetData().eventType_ == GameEventType::kStart) {
			cameraManager->SetUseCamera("stageCamera", 0.0f);
			isStart = true;
			if (eventStateMachine_->GetCurrentState()->RemainingTime() <= 0.5f) {
				gameStartUI_->IsFade();
			}
		}
		else {
			if (isStart) {
				gameStartUI_->IsNofade();
				isStart = false;
				cameraManager->SetUseCamera("followCamera", 0.0f);
			}
		}

		if (eventStateMachine_->GetCurrentState()->GetData().eventType_ == GameEventType::kBreakTime) {
			gameStartUI_->IsNum(true);
		}
		else {
			gameStartUI_->IsNum(false);
		}


		
		bool isCharaPlayerDed = !characterManager->GetPlayer()->GetAlive();
		bool isCharaEnemyDed = characterManager->GetCharacterCount(Character::Type::Enemy) <= 0;

		bool isEndState = eventStateMachine_->IsEventFinished();
		bool isFinish = eventStateMachine_->GetCurrentState()->IsFinish();


		if (isCharaPlayerDed) {
			playerDieTime += dt;
			if (playerDieTime >= 1.0f)
 			isEndEvent_ = true;
		}

		// 最後のステートでなおかつ終了状態なら
		if (isEndState && isFinish) {
			isEndEvent_ = true;
		}

	}

	void GameEventController::Draw() {

	};

	void GameEventController::Draw2D() {
		gameStartUI_->Draw();
	};


}