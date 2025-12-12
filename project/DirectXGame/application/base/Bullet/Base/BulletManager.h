#pragma once
// 
#include"DirectXGame/application/base/Bullet/Base/BaseBullet.h"


// 前方宣言
class BasePlayer;
class BaseEnemy;
namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
	class GlobalVariables;
	class Camera;
}


/// <summary>
/// 弾管理クラス
/// </summary>
class BulletManager {
public:
	~BulletManager();

	// 弾の種類
	enum class BulletType
	{
		kPlayerMissile,	// プレイヤーミサイル
		kRangeBombingSpecial,	// レンジボミングスペシャル
		kPlayerBullet,	// プレイヤー弾
		kPlayerStan,			// スタン
		kEnemyBullet,

	};

	// 弾の状態
	enum class BulletBehavior 
	{
		kFollow,				// ターゲットを追尾する弾
		kStraight,				// 直進する弾
		kStraightWithGravity,	// 重力を受けて直進する弾
		kRange,					// 範囲攻撃弾
	};

	// 初期化
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Engine::Camera* camera);
	

	// 更新
	void Update();

	// 描画3D
	void Draw();
	
	// 描画エフェクト
	void DrawEffect();

	// 描画2D
	void Draw2D();



public: // 生成

	// 範囲攻撃する弾を生成(絨毯爆撃)
	void GenerateBulletRange(BulletType type, Vector3 position, Vector3 targetPos, float rad);

	// 弾を生成
	void GenerateBullet(BulletType type, const BulletInfo& info, BaseEnemy* enemy = nullptr);

public: // 取得or設定
	// 弾リストを取得
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const { return bullets_; }
		
	// プレイヤーをセット
	void SetPlayer(BasePlayer* player) { player_ = player; };

	// エフェクトの設定
	void SetEffect(Effect* effect) { effect_ = effect; }
private:

	// 弾
	std::list<std::unique_ptr<BaseBullet>> bullets_;


private:
	BasePlayer* player_;							// プレイヤー
	Effect* effect_;								// 演出
	Engine::Camera* camera_;								// カメラ
	Engine::GlobalVariables* globalVariables_ = nullptr;	// 保存項目
	Engine::Entity3DManager* entity3DManager_;	// 3dオブジェクト管理
	Engine::Entity2DManager* entity2DManager_;  // 2Dオブジェクト管理
};
