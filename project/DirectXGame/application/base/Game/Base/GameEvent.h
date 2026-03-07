#pragma once
#include "GameData.h"
#include "map"
#include "vector"
#include "DirectXGame/engine/input/Input.h"

// 前方宣言
namespace Character {
	class CharacterManager;
	class CharacterSpawnManager;
	class SpawnInfo;
}
namespace Engine {
	class GlobalVariables;
	class SceneManager;
	class EntityManager;
	class Input;
}

namespace Game {

	/// <summary>
	/// ゲームイベントの種類
	/// </summary>
	enum class GameEventType {
		kNone,		// 無し
		kBattle,	// 戦い
		kBreakTime,	// 休憩時間
		kEnd,		// 終了
	};

	/// <summary>
	/// ゲームイベントの変更するタイプ
	/// </summary>
	enum class GameEventChangeType {
		kTime,	// 時間経過で
		kKill,	// 全滅させたら
		kTake,	// 何かを取得したら
		kInput,	// 何か入力があったら
	};

	/// <summary>
	/// ゲームイベント内での時間に関する構造体
	/// </summary>
	struct GameEventTime {
		// 最大経過時間(次のゲームイベントに移行する時間)
		float max = 1.0f;
	};


	/// <summary>
	/// 倒して変わるタイプ
	/// </summary>
	enum class GameEvemtKillChangeType {
		kAll,	// 全て
		kCount,	// 一定数
	};

	/// <summary>
	/// ゲームイベント内での
	/// </summary>
	struct GameEventKillChange
	{
		// 倒して変更するタイプ
		GameEvemtKillChangeType type;
		// 倒すキャラクターの数
		int killCharacterCount;
	};

	/// <summary>
	/// イベントデータ
	/// </summary>
	struct GameEventData {
		// ゲームイベントのタイプ
		GameEventType eventType_ = GameEventType::kBattle;
		// ゲームイベントの変更するタイプ
		GameEventChangeType changeType_ = GameEventChangeType::kTime;
		// ゲームイベントの倒して変更するタイプの情報
		GameEventKillChange killChange_;
		// ゲームイベント内での時間に関する構造体
		GameEventTime time_;
		// バトルウェーブ番号
		int battleWaveIndex_ = 0;
		// 入力
		GamePadButton buttom = GamePadButton::GAMEPAD_B;
		// 敵削除
		bool enemyDelete = false;
	};


	

	/// <summary>
	/// ゲームイベント
	/// </summary>
	class GameEventState {
	public:
		~GameEventState() {
			spawns_.clear();
		}

		// 開始
		void Enter(Character::CharacterManager* characterManager, Character::CharacterSpawnManager* characterSpawnManager
			, Engine::Input* input);
		// 終了
		void Exit();

		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="dt"></param>
		void Update(float dt);

	public:
		// 次のステート
		void SetNextState(std::shared_ptr<GameEventState> next) {
			nextStates[GetNextStateName()] = next;
		}
		// 次のステートがあったら
		std::shared_ptr<GameEventState> HandleInput() {
			auto it = nextStates.find(GetNextStateName());
			if (it != nextStates.end()) {
				return it->second;
			}
			return nullptr;
		}
		// 次のステートの名前取得
		std::string GetNextStateName() const;
		// 次のステートは存在するか
		bool HasNextState() const {	return !nextStates.empty();}
		// データ設定
		void SetData(const GameEventData& data) { data_ = data; };
		// データ取得
		GameEventData GetData() const { return data_; };
		// コンボ名取得
		std::string GetName() const { return name; }
		// コンボ名設定
		void SetName(const std::string& comboName) { name = comboName; }
		// スポーン追加
		void AddSpawns(std::vector<Character::SpawnInfo>&& spawns);
		// 
		float GetCurrentTimer() const;
	public:
		// 終了したか
		bool IsFinish() const { return isFinish_; };
		// ゲームイベントの変更するタイプ取得
		GameEventChangeType GetChangeType() const { return data_.changeType_; }
	private:
		// 終了条件処理
		bool FinishProcess();
		// 出現処理
		void SpawnProcess();

	private:
		// コンボ名
		std::string name;
		// ゲームイベントデータ
		GameEventData data_;
		// 終了下か
		bool isFinish_ = false;
		// 時間
		float stateTime = 0.0f;
	private: // 貰いもの
		// 次のステートマップ
		std::map<std::string, std::shared_ptr<GameEventState>> nextStates;
		// 出現情報
		std::vector<Character::SpawnInfo> spawns_;
		// 
		Engine::EntityManager* entityManager = nullptr;
		Character::CharacterManager* characterManager = nullptr;
		Character::CharacterSpawnManager* characterSpawnManager = nullptr;
		Engine::GlobalVariables* globalVariables = nullptr;
		Engine::SceneManager* sceneManager = nullptr;
		Engine::Input* input = nullptr;
	};

	/// <summary>
	/// イベントステートマシン
	/// </summary>
	class EventStateMachine {
	public:
		// 初期化
		void Initialize(Character::CharacterManager* characterManager, Character::CharacterSpawnManager* characterSpawnManager
			,Engine::Input* input);


		// ステート設定
		void SetState(std::shared_ptr<GameEventState> state);
		// 更新
		void Update(float dt);


		// リセット
		void Reset() { SetState(rootState); }
		// 設定
		void SetRoot(std::shared_ptr<GameEventState> state);
		// コンボが終了したか
		bool IsEventFinished() const {
			auto node = std::dynamic_pointer_cast<GameEventState>(currentState);
			if (!node) return true;
			return !node->HasNextState();
		}

		std::shared_ptr<GameEventState> GetCurrentState() { return currentState; }
	private:
		std::shared_ptr<GameEventState> currentState;   // 現在のステート
		std::shared_ptr<GameEventState> rootState;      // 初期ステート


		Engine::EntityManager* entityManager = nullptr;
		Character::CharacterManager* characterManager = nullptr;
		Character::CharacterSpawnManager* characterSpawnManager = nullptr;
		Engine::GlobalVariables* globalVariables = nullptr;
		Engine::SceneManager* sceneManager = nullptr;
		Engine::Input* input = nullptr;
	};



}