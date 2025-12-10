#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>

// 前方宣言
class BaseCharacterManager;
class LineCommon;



/// <summary>
/// キャラクターの出現情報
/// </summary>
class SpawnInfo {
public:


	struct Data
	{
		std::string name_ = "";				// 名前
		int spawnMaxCount_ = 1;				// 出現最大回数
		int spawnAmount_ = 0;				// 出現量
		Vector3 size_ = { 1.0f,1.0f,1.0f }; // 出現エリアサイズ
		float spawnInterval_ = 1.0f;        // 出現間隔
		float spawnTimer_ = 0.0f;           // 出現タイミングタイマー(フェーズが始まってから)
	};

	// 初期化
	void Initialize(const std::string& name, int spawnMaxCount , int spawnAmount) {
		data_.name_ = name;					// 名前
		data_.spawnMaxCount_ = spawnMaxCount;	// 出現回数
		data_.spawnAmount_ = spawnAmount;		// 出現量
	}


	// 更新
	void Update(float timer) {
		spawnDelay_ += timer;	//
		// 出現インターバル
		if (spawnDelay_ >= data_.spawnInterval_) {
			spawnDelay_ = 0.0f;
			isSpawned_ = false;
		}
	}

	// 出現処理
	void Spawned(){
		spawnCount_++;

		isSpawned_ = true;

		if (spawnCount_ >= data_.spawnMaxCount_) {
			isEnd_ = true;
		}
	}

	// 出現終了かどうか
	bool IsEnd() const { return isEnd_; }

	// 出現済みかどうか
	bool IsSpawned() const { return isSpawned_; }

	// 出現可能かどうか
	bool IsSpawn() const { return isSpawn_; }

	// 出現フラグ設定
	void SetIsSpawn(bool isSpawn) { isSpawn_ = isSpawn; }

	// データ取得
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
	// 初期化
	void Initialize(BaseCharacterManager* characterManager, LineCommon* line, const SpawnInfo& info);

	// 更新
	void Update(float time);

	// 描画
	void Draw();

public: // 取得or設定

	// 名前取得
	std::string GetName() const { return spawnInfo_.GetData().name_; }

	// 出現エリア設定
	void SetSizeArea(const Vector3& size) { spawnInfo_.GetData().size_ = size; }

	// 出現量設定
	void SetSpawnAmount(int amount) { spawnInfo_.GetData().spawnAmount_ = amount; }

	// 出現情報取得
	SpawnInfo& GetSpawnInfo() { return spawnInfo_; }

	WorldTransform& GetSpawnTransform() { return spawnTransform_; }
public:
	// 出現処理
	void SpawnProcess();
private:
	SpawnInfo spawnInfo_;				// 出現情報
	AABB spawnAABBArea_;				// 出現エリアAABB	
	WorldTransform spawnTransform_;		// 出現位置変換行列

	float timer_ = 0.0f;                // タイマー
private:
	BaseCharacterManager* characterManager_ = nullptr;
	LineCommon* lineCommon_ = nullptr;
};

