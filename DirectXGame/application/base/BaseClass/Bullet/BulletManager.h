#pragma once



// engine
#include"DirectXGame/engine/Camera/Camera.h"

// 
#include"DirectXGame/application/base/BaseClass/Bullet/BaseBullet.h"



class BasePlayer;
class BaseEnemy;
class Entity3DManager;
class Entity2DManager;

class BulletManager {
public:
	enum class BulletType
	{
		kPlayerMissile,	// プレイヤーミサイル
		kRangeBombingSpecial,	// レンジボミングスペシャル
		kEnemyBullet,
	};

	enum class BulletBehavior 
	{
		kFollow,				// ターゲットを追尾する弾
		kStraight,				// 直進する弾
		kStraightWithGravity,	// 重力を受けて直進する弾
		kRange,					// 範囲攻撃弾
	};

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera);
	void SetPlayer(BasePlayer* player) { player_ = player; };


	// 更新
	void Update();

	// 描画3D
	void Draw();
	
	// 描画3D
	void DrawEffect();

	// 描画2D
	void Draw2D();

	// 範囲攻撃する弾を生成(絨毯爆撃)
	void GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad);

	// ターゲットを追尾する弾を生成
	void GenerateBullet(BulletType type, Vector3 position, BaseEnemy* enemy = nullptr);

	// 弾リストを取得
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const { return bullets_; }
private:
	std::list<std::unique_ptr<BaseBullet>> bullets_;



private:
	BasePlayer* player_;
	Camera* camera_;						// カメラ
	Entity3DManager* entity3DManager_;	// 3dオブジェクト管理
	Entity2DManager* entity2DManager_;  // 2Dオブジェクト管理
};
