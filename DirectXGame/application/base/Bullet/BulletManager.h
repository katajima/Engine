#pragma once



// engine
#include"DirectXGame/engine/Camera/Camera.h"

// 
#include"DirectXGame/application/base/Bullet/BaseBullet.h"



class Player;
class BaseEnemy;
class Entity3DManager;
class Entity2DManager;

class BulletManager {
public:
	enum class BulletType
	{
		kPlayerMissile,	// プレイヤーミサイル
		kEnemyBullet,
	};

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera);
	void SetPlayer(Player* player) { player_ = player; };


	// 更新
	void Update();

	// 描画3D
	void Draw();

	// 描画2D
	void Draw2D();

	void GeneratBullet(BulletType type, Vector3 position, BaseEnemy* enemy = nullptr);

	// 弾リストを取得
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const { return bullets_; }
private:
	std::list<std::unique_ptr<BaseBullet>> bullets_;



private:
	Player* player_;
	Camera* camera_;						// カメラ
	Entity3DManager* entity3DManager_;	// 3dオブジェクト管理
	Entity2DManager* entity2DManager_;  // 2Dオブジェクト管理
};
