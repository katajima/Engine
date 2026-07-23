#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include <DirectXGame/application/base/Character/Base/CharacterData.h>
#include <DirectXGame/application/base/Character/Enemy/Base/EnemyData.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyCrowdBehaviorData.h>

namespace Engine {
	class LineCommon;
}

namespace Character {
	// 前方宣言
	class CharacterManager;

	/// <summary>
	/// キャラクターの出現情報
	/// </summary>
	class SpawnInfo {
	public:


		struct Data
		{
			EnemyType type_ = EnemyType::kMediumMelee;// 敵タイプ
			std::string name_ = "";				// 名前
			int spawnMaxCount_ = 1;				// 出現最大回数
			int spawnAmount_ = 0;				// 出現量
			Vector3 translate_ = { 0,0,0 };		// 位置
			Vector3 size_ = { 1.0f,1.0f,1.0f }; // 出現エリアサイズ
			float spawnInterval_ = 1.0f;        // 出現間隔
			float spawnTimer_ = 0.0f;           // 出現タイミングタイマー(フェーズが始まってから)
			int maxEnemyCount_ = 30;			// 最大敵出現量
			float startDelay_ = 0.0f;           // フェーズ開始からの遅延時間
			int groupId_ = 0;					// グループID
			CrowdBehaviorSettings crowdBehavior_{};	// この群衆が使用する行動パターン
		};

		// 初期化
		void Initialize(const std::string& name, int spawnMaxCount, int spawnAmount, Vector3 translate,
			Vector3 size = { 10,1,10 }, float interval = 1.0f, float startDelay = 0.0f, int groupId = 0,
			const CrowdBehaviorSettings& crowdBehavior = CrowdBehaviorSettings::Flocking()) {
			data_.name_ = name;						// 名前
			data_.spawnMaxCount_ = spawnMaxCount;	// 出現回数
			data_.spawnAmount_ = spawnAmount;		// 出現量
			data_.size_ = size;						// サイズ
			data_.translate_ = translate;			// 位置
			data_.spawnInterval_ = interval;		// インターバル
			data_.startDelay_ = startDelay;			// 開始遅延時間
			data_.groupId_ = groupId;				// グループID
			data_.crowdBehavior_ = crowdBehavior;	// 行動パターン
		}


		/// <summary>
		/// 更新
		/// </summary>
		void Update(float timer) {
			spawnDelay_ += timer;	//
			/// <summary>
			/// 出現インターバル
			/// </summary>
			if (spawnDelay_ >= data_.spawnInterval_) {
				spawnDelay_ = 0.0f;
				isSpawned_ = false;
			}
		}

		/// <summary>
		/// 出現処理
		/// </summary>
		void Spawned() {
			spawnCount_++;

			isSpawned_ = true;

			if (spawnCount_ >= data_.spawnMaxCount_) {
				isEnd_ = true;
			}
		}

		/// <summary>
		/// 出現終了かどうか
		/// </summary>
		bool IsEnd() const { return isEnd_; }

		/// <summary>
		/// 出現済みかどうか
		/// </summary>
		bool IsSpawned() const { return isSpawned_; }

		/// <summary>
		/// 出現可能かどうか
		/// </summary>
		bool IsSpawn() const { return isSpawn_; }

		/// <summary>
		/// 出現フラグ設定
		/// </summary>
		void SetIsSpawn(bool isSpawn) { isSpawn_ = isSpawn; }

		/// <summary>
		/// データ取得
		/// </summary>
		Data& GetData() { return data_; }

		const Data& GetData() const { return data_; }

	private:
		Data data_;
	private:
		bool isSpawn_ = false;				// 出現フラグ
		int spawnCount_ = 0;				// 出現回数
		float spawnDelay_ = 0.0f;           // 出現遅延時間
		bool isSpawned_ = false;			// 出現済みフラグ
		bool isEnd_ = false;				// 出現終了フラグ
	};

	/// <summary>
	/// キャラクター出現クラス
	/// </summary>
	class CharacterSpawn {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(CharacterManager* characterManager, Engine::LineCommon* line, const SpawnInfo& info);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(float time);

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

	public: // 取得or設定
		/// <summary>
		/// 名前取得
		/// </summary>
		std::string GetName() const { return spawnInfo_.GetData().name_; }
		/// <summary>
		/// 出現エリア設定
		/// </summary>
		void SetSizeArea(const Vector3& size) { spawnInfo_.GetData().size_ = size; }
		/// <summary>
		/// 出現量設定
		/// </summary>
		void SetSpawnAmount(int amount) { spawnInfo_.GetData().spawnAmount_ = amount; }
		/// <summary>
		/// 出現情報取得
		/// </summary>
		SpawnInfo& GetSpawnInfo() { return spawnInfo_; }
		/// <summary>
		/// 出現位置取得
		/// </summary>
		Engine::WorldTransform& GetSpawnTransform() { return spawnTransform_; }
		/// <summary>
		/// 最大敵出現量設定
		/// </summary>
		void SetMaxEnemyCount(int count) { maxEnemyCount_ = count; }

	public:
		/// <summary>
		/// 出現処理
		/// </summary>
		void SpawnProcess();
	private:
		SpawnInfo spawnInfo_;				// 出現情報
		AABB spawnAABBArea_;					// 出現エリアAABB	
		Engine::WorldTransform spawnTransform_;		// 出現位置変換行列

		float timer_ = 0.0f;                // タイマー
		int maxEnemyCount_ = 30;			// 最大敵出現量
	private:
		CharacterManager* characterManager = nullptr;
		Engine::LineCommon* lineCommon = nullptr;
	};
}
