#include "GameEventController.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManager.h"

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

		// スポーン地点はプレイヤーを中心に四方向へ用意し、ウェーブの攻め方に合わせて使い分ける
		const Vector3 frontSpawn = { 0, 1, 50 };
		const Vector3 backSpawn = { 0, 1, -50 };
		const Vector3 rightSpawn = { 50, 1, 0 };
		const Vector3 leftSpawn = { -50, 1, 0 };
		const Vector3 wideSpawnSize = { 24, 1, 24 };
		const Vector3 narrowSpawnSize = { 14, 1, 14 };

		data.eventType_ = GameEventType::kBattle;
		data.changeType_ = GameEventChangeType::kTime;
		// 各バトルの後に退場フェーズを挟み、残存敵は自然に消してから次の群衆を出す
		data.enemyDelete = false;
		data.enemyWaveExit = false;

		// WAVE 1: 正面から来る小規模群衆。直進と群れ移動を覚える導入（2群衆 / 合計8体）
		const Character::CrowdBehaviorSettings wave1Rush = Character::CrowdBehaviorSettings::Rush();
		const Character::CrowdBehaviorSettings wave1Flock = Character::CrowdBehaviorSettings::Flocking();
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave1Rush", 1, 30, frontSpawn, narrowSpawnSize, 1.0f, 0.0f, 10, wave1Rush);
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave2Rush", 1, 30, rightSpawn, narrowSpawnSize, 1.0f, 0.0f, 10, wave1Rush);
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave3Rush", 1, 30, leftSpawn, narrowSpawnSize, 1.0f, 0.0f, 10, wave1Rush);
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave4Rush", 1, 30, backSpawn, narrowSpawnSize, 1.0f, 0.0f, 10, wave1Rush);

		
		CreateSpawn(Character::EnemyType::kSmallRanged, "wave1Flock", 1, 30, backSpawn, narrowSpawnSize, 1.0f, 0.0f, 11, wave1Flock);
		data.battleWaveIndex_ = 1;
		data.time_.max = 24.0f;
		CreateGameEvent("battle01", data);
		data.eventType_ = GameEventType::kWaveExit;
		data.enemyWaveExit = false;
		data.time_.max = 1.2f;
		CreateGameEvent("waveExit01", data);
		data.eventType_ = GameEventType::kBattle;
		data.enemyWaveExit = false;

		// WAVE 2: 左右から寄せ、片側にはあえて逃げ道を残す（2群衆 / 合計13体）
		const Character::CrowdBehaviorSettings wave2Encircle = Character::CrowdBehaviorSettings::Encircle(12.0f);
		const Character::CrowdBehaviorSettings wave2Distributed = Character::CrowdBehaviorSettings::DistributedEncircle(15.0f);
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave2Encircle", 1, 50, leftSpawn, wideSpawnSize, 1.0f, 0.0f, 20, wave2Encircle);
		CreateSpawn(Character::EnemyType::kSmallRanged, "wave2Distributed", 1, 20, rightSpawn, narrowSpawnSize, 1.0f, 0.0f, 21, wave2Distributed);
		data.battleWaveIndex_ = 2;
		data.time_.max = 28.0f;
		CreateGameEvent("battle02", data);
		data.eventType_ = GameEventType::kWaveExit;
		data.enemyWaveExit = false;
		data.time_.max = 1.2f;
		CreateGameEvent("waveExit02", data);
		data.eventType_ = GameEventType::kBattle;
		data.enemyWaveExit = false;

		// WAVE 3: 前後から列が入れ替わって攻める波状攻撃（2群衆 / 合計17体）
		const Character::CrowdBehaviorSettings wave3Assault = Character::CrowdBehaviorSettings::WaveAssault(2.2f, 3);
		const Character::CrowdBehaviorSettings wave3Line = Character::CrowdBehaviorSettings::Formation(Character::EnemyFormationShape::Line);
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave3Assault", 1, 100, frontSpawn, wideSpawnSize, 1.0f, 0.0f, 30, wave3Assault);
		CreateSpawn(Character::EnemyType::kSmallRanged, "wave3Line", 1, 50, backSpawn, narrowSpawnSize, 1.0f, 0.0f, 31, wave3Line);
		data.battleWaveIndex_ = 3;
		data.time_.max = 32.0f;
		CreateGameEvent("battle03", data);
		data.eventType_ = GameEventType::kWaveExit;
		data.enemyWaveExit = false;
		data.time_.max = 1.2f;
		CreateGameEvent("waveExit03", data);
		data.eventType_ = GameEventType::kBattle;
		data.enemyWaveExit = false;

		// WAVE 4: 流動する外周と役割分担する追跡隊で移動を強制する（2群衆 / 合計20体）
		const Character::CrowdBehaviorSettings wave4Flow = Character::CrowdBehaviorSettings::FlowCrowd(13.0f);
		const Character::CrowdBehaviorSettings wave4Hunting = Character::CrowdBehaviorSettings::Hunting();
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave4Flow", 1, 75, leftSpawn, wideSpawnSize, 1.0f, 0.0f, 40, wave4Flow);
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave4Hunters", 1, 40, rightSpawn, narrowSpawnSize, 1.0f, 0.0f, 41, wave4Hunting);
		CreateSpawn(Character::EnemyType::kSmallRanged, "wave4Support", 1, 20, rightSpawn, narrowSpawnSize, 1.0f, 0.0f, 41, wave4Hunting);
		data.battleWaveIndex_ = 4;
		data.time_.max = 36.0f;
		CreateGameEvent("battle04", data);
		data.eventType_ = GameEventType::kWaveExit;
		data.enemyWaveExit = false;
		data.time_.max = 1.2f;
		CreateGameEvent("waveExit04", data);
		data.eventType_ = GameEventType::kBattle;
		data.enemyWaveExit = false;

		// WAVE 5: 圧迫群衆、人数適応群衆、V字の中型敵を同時投入する最終戦（3群衆 / 合計28体）
		const Character::CrowdBehaviorSettings wave5Pressure = Character::CrowdBehaviorSettings::PressureCrowd();
		const Character::CrowdBehaviorSettings wave5Adaptive = Character::CrowdBehaviorSettings::DensityAdaptive();
		const Character::CrowdBehaviorSettings wave5Vanguard = Character::CrowdBehaviorSettings::Formation(Character::EnemyFormationShape::VShape);
		CreateSpawn(Character::EnemyType::kSmallMelee, "wave5Pressure", 1, 50, frontSpawn, wideSpawnSize, 1.0f, 0.0f, 50, wave5Pressure);
		CreateSpawn(Character::EnemyType::kSmallRanged, "wave5Adaptive", 1, 16, backSpawn, wideSpawnSize, 1.0f, 0.0f, 51, wave5Adaptive);
		CreateSpawn(Character::EnemyType::kMediumMelee, "wave5Vanguard", 1, 24, rightSpawn, narrowSpawnSize, 1.0f, 0.0f, 52, wave5Vanguard);
		data.battleWaveIndex_ = 5;
		data.time_.max = 45.0f;
		CreateGameEvent("battle05", data);
		data.eventType_ = GameEventType::kWaveExit;
		data.enemyWaveExit = false;
		data.time_.max = 1.2f;
		CreateGameEvent("waveExit05", data);

		data.changeType_ = GameEventChangeType::kTime;
		data.eventType_ = GameEventType::kEnd;
		data.time_.max = 3.0f;
		data.enemyDelete = false;
		data.enemyWaveExit = false;

		CreateGameEvent("result", data);

		ConnectNode("start", "", "breakTime");
		ConnectNode("breakTime", "", "battle01");
		ConnectNode("battle01", "", "waveExit01");
		ConnectNode("waveExit01", "", "battle02");
		ConnectNode("battle02", "", "waveExit02");
		ConnectNode("waveExit02", "", "battle03");
		ConnectNode("battle03", "", "waveExit03");
		ConnectNode("waveExit03", "", "battle04");
		ConnectNode("battle04", "", "waveExit04");
		ConnectNode("waveExit04", "", "battle05");
		ConnectNode("battle05", "", "waveExit05");
		ConnectNode("waveExit05", "", "result");
		
		eventStateMachine_->SetRoot(GetNodeState("start"));
	}


	void GameEventController::CreateSpawn(Character::EnemyType type, const std::string& name, int spawnMaxCount, int spawnAmount,
		Vector3 translate, Vector3 size, float interval, float startDelay, int groupId,
		const Character::CrowdBehaviorSettings& crowdBehavior) {
		Character::SpawnInfo data;
		data.GetData().type_ = type;

		data.Initialize(name, spawnMaxCount, spawnAmount, translate, size, interval, startDelay, groupId, crowdBehavior);
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


		
		bool isCharaPlayerDed = false;
		if (characterManager->GetPlayer()) {
			isCharaPlayerDed = !characterManager->GetPlayer()->GetAlive();
		}
		else {
			isCharaPlayerDed = true;
		}


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
