#pragma once
// 
#include"DirectXGame/application/base/Bullet/Base/BaseBullet.h"


// 前方宣言
class BasePlayer;
class BaseEnemy;
class BaseCharacter;
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

	// 弾の追加
	void AddBullet(std::unique_ptr<BaseBullet> bullet);

public: // 取得or設定
	// 弾リストを取得
	const std::list<std::unique_ptr<BaseBullet>>& GetBullets() const { return bullets_; }
		
	// エフェクトの設定
	void SetEffect(Effect* effect) { effect_ = effect; }
private:

	// 弾
	std::list<std::unique_ptr<BaseBullet>> bullets_;


private:
	Effect* effect_;								// 演出
	Engine::Camera* camera_;								// カメラ
	Engine::GlobalVariables* globalVariables_ = nullptr;	// 保存項目
	Engine::Entity3DManager* entity3DManager_;	// 3dオブジェクト管理
	Engine::Entity2DManager* entity2DManager_;  // 2Dオブジェクト管理
};
