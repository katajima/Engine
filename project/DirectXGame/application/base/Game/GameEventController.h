#pragma once
#include <DirectXGame/application/base/Character/Spawn/CharacterSpawnManager.h>
#include "DirectXGame/application/base/Game/Base/GameEvent.h"
#include "DirectXGame/application/base/UI/GameStartUI.h"

// 前方宣言
namespace Engine {
	class EntityManager;
	class GlobalVariables;
	class Input;
}

namespace Game {

	/// <summary>
	/// ゲームのイベントを管理するクラス
	/// </summary>
	class GameEventController {
	public:
		~GameEventController() {
			spawnInfos_.clear();
		}
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Engine::EntityManager* entity3DManager, Engine::GlobalVariables* globalVariables, 
			CameraManager* cameraManager, Character::CharacterManager* characterManager, InputSystem* input);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(float dt);

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

		/// <summary>
		/// 描画2d
		/// </summary>
		void Draw2D();

	public:
		// イベント終了通知
		bool IsEndEvent() const { return isEndEvent_; };
		// 移動可能か
		bool IsMove() const { return eventStateMachine_->GetCurrentState()->GetData().isMove; }

		// 現在のウェーブ
		int GetCurrentWave() const { return curretWave_; }
		// 現在の時間
		float GetCurrentWaveTime() const { return time_; }
		// スポーン生成
		void CreateSpawn(Character::EnemyType type, const std::string& name, int spawnMaxCount, int spawnAmount,
			Vector3 translate, Vector3 size = { 10,1,10 }, float interval = 1.0f, float startDelay = 0.0f, int groupId = 0,
			const Character::CrowdBehaviorSettings& crowdBehavior = Character::CrowdBehaviorSettings::Flocking());
	public: // イベント系

		// ノード追加(データから生成)
		void AddNode(const std::string& nodeName, const GameEventData& data);
		// ノード連結
		void ConnectNode(const std::string& from, const std::string& name, const std::string& to);

		/// <summary>
		/// イベント生成
		/// </summary>
		void CreateGameEvent(const std::string& nodeName, const GameEventData& data);

		// コンボノードステート取得
		std::shared_ptr<GameEventState> GetNodeState(const std::string& name) {
			auto it = eventStateNodes_.find(name);
			if (it != eventStateNodes_.end()) {
				return it->second;
			}
			return nullptr;
		}
	private:
		// ゲームイベントステートマシン
		std::unique_ptr<EventStateMachine> eventStateMachine_ = nullptr;
		// ゲームイベントステートマップ
		std::map<std::string, std::shared_ptr<GameEventState>> eventStateNodes_;
		// キャラクター出現管理クラス
		std::unique_ptr<Character::CharacterSpawnManager> characterSpawnManager_ = nullptr;
		// スポーン情報
		std::vector<Character::SpawnInfo> spawnInfos_;
		bool isEndEvent_ = false;	// イベント終了フラグ
		int curretWave_ = 0;		// ウェーブ
		float time_ = 0.0f;			// 時間
	private:
		// スタートUI
		std::unique_ptr<GameStartUI> gameStartUI_ = nullptr;
		//
		bool isStart = false;


		float playerDieTime = 0.0f;
	private:
		// エンティティ管理
		Engine::EntityManager* entityManager = nullptr;
		// 保存項目
		Engine::GlobalVariables* globalVariables = nullptr;
		// キャラクター管理
		Character::CharacterManager* characterManager = nullptr;
		// カメラ管理
		CameraManager* cameraManager = nullptr;
	};

}
