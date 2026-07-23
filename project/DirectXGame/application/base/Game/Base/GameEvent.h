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
class InputSystem;

namespace Game {

	/// <summary>
	/// ゲームイベントの種類
	/// </summary>
	enum class GameEventType {
		kNone,		// 無し
		kBattle,	// 戦い
		kBreakTime,	// 休憩時間
		kWaveExit,	// ウェーブ終了時の敵退場
		kStart,		// スタート
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
		// 敵を自然退場ステートへ移行する
		bool enemyWaveExit = false;
		// 移動可能か
		bool isMove = true;
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
			, InputSystem* input);
		/// <summary>
		/// 終了
		/// </summary>
		void Exit();

		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="dt"></param>
		void Update(float dt);

	public:
		/// <summary>
		/// 次のステート
		/// </summary>
		void SetNextState(std::shared_ptr<GameEventState> next) {
			nextStates[GetNextStateName()] = next;
		}
		/// <summary>
		/// 次のステートがあったら
		/// </summary>
		std::shared_ptr<GameEventState> HandleInput() {
			auto it = nextStates.find(GetNextStateName());
			if (it != nextStates.end()) {
				return it->second;
			}
			return nullptr;
		}
		/// <summary>
		/// 次のステートの名前取得
		/// </summary>
		std::string GetNextStateName() const;
		/// <summary>
		/// 次のステートは存在するか
		/// </summary>
		bool HasNextState() const {	return !nextStates.empty();}
		/// <summary>
		/// データ設定
		/// </summary>
		void SetData(const GameEventData& data) { data_ = data; };
		/// <summary>
		/// データ取得
		/// </summary>
		GameEventData GetData() const { return data_; };
		/// <summary>
		/// コンボ名取得
		/// </summary>
		std::string GetName() const { return name; }
		/// <summary>
		/// コンボ名設定
		/// </summary>
		void SetName(const std::string& comboName) { name = comboName; }
		/// <summary>
		/// スポーン追加
		/// </summary>
		void AddSpawns(std::vector<Character::SpawnInfo>&& spawns);
		/// <summary>
		/// 現在の経過時間
		/// </summary>
		float GetCurrentTimer() const;
		/// <summary>
		/// 残り時間
		/// </summary>
		float RemainingTime() const;
	public:
		/// <summary>
		/// 終了したか
		/// </summary>
		bool IsFinish() const { return isFinish_; };
		/// <summary>
		/// ゲームイベントの変更するタイプ取得
		/// </summary>
		GameEventChangeType GetChangeType() const { return data_.changeType_; }
	private:
		/// <summary>
		/// 終了条件処理
		/// </summary>
		bool FinishProcess();
		/// <summary>
		/// 出現処理
		/// </summary>
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
		InputSystem* input = nullptr;
	};

	/// <summary>
	/// イベントステートマシン
	/// </summary>
	class EventStateMachine {
	public:
		// 初期化
		void Initialize(Character::CharacterManager* characterManager, Character::CharacterSpawnManager* characterSpawnManager
			, InputSystem* input);


		/// <summary>
		/// ステート設定
		/// </summary>
		void SetState(std::shared_ptr<GameEventState> state);
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float dt);


		/// <summary>
		/// リセット
		/// </summary>
		void Reset() { SetState(rootState); }
		/// <summary>
		/// 設定
		/// </summary>
		void SetRoot(std::shared_ptr<GameEventState> state);
		/// <summary>
		/// コンボが終了したか
		/// </summary>
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
		InputSystem* input = nullptr;
	};



}
