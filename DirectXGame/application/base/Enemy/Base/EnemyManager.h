#pragma once

// engine
#include"DirectXGame/engine/Camera/Camera.h"

// アプリケーション
#include"BaseEnemy.h"
#include"DirectXGame/application/base/Enemy/Normal/NormalEnemy.h"


/// <summary>
/// 前方宣言
/// </summary>
class Player;
class Entity3DManager;
class Entity2DManager;
/// <summary>
/// 敵を管理するマネージャ
/// </summary>
class EnemyManager {
public:
	enum class EnemyType
	{
		kNormal,	// 通常
		kBullet,	// 弾撃ち
		kAttacker,	// アタッカー 
		kTank,		// タンク
		kElite,     // エリート
		kBoss,		// ボス 
	};


	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera);
	void SetPlayer(Player* player) { player_ = player; };
	
	// 更新
	void Update();

	
	// 描画2D
	void Draw2D();



	// 生成
	void GenerateEnemy(EnemyType type, Vector3 position);

	std::vector<BaseEnemy*> GetEnemys() {
		std::vector<BaseEnemy*> result;
		for (auto& enemy : enemys_) {
			result.push_back(enemy.get());
		}
		return result;
	}
private:
	std::vector<std::unique_ptr<BaseEnemy>> enemys_;
private:
	Camera* camera_;						// カメラ
	Entity3DManager* entity3DManager_;	// 3dオブジェクト管理
	Entity2DManager* entity2DManager_;  // 2Dオブジェクト管理
	Player* player_;					// プレイヤー
};
