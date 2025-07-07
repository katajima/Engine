#pragma once
#include "DirectXGame/application/base/BaseClass/Special/BaseSpecial.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"

class Entity3DManager;
class Entity2DManager;
class BulletManager;
class FollowCamera;
class BaseEnemy;
class Stage;
class RangeBombingSpecial : public BaseSpecial
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	///< summary>
	/// 描画
	///</summary>
	void Draw() override;
public:

	/// <summary>
	/// 発動中
	/// </summary>
	void InAction(FollowCamera* followCamera, BulletManager* bulletManager ,Vector3 worldpos,float rad);

public:

	

	void SetStage(Stage* stage);

	bool IsAction() const { return isAction_; }

private:
	int index_b = 0;
	int maxBullet = 1;
	int bulletNum = 0;
	int currentMissileIndex = 0;

	float shotTimer = 0.0f;

	bool isAction_ = false;

private:
	Stage* stage_ = nullptr;

};

