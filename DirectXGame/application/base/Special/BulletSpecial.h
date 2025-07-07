#pragma once
#include "DirectXGame/application/base/BaseClass/Special/BaseSpecial.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"

class Entity3DManager;
class Entity2DManager;
class BulletManager;
class FollowCamera;
class BaseEnemy;
class BulletSpecial : public BaseSpecial
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
	void InAction(FollowCamera* followCamera,BulletManager* bulletManager,std::vector<BaseEnemy*> enemy);

public:

	void SetParent(WorldTransform* parent);

	

private:
	// ミサイル発射位置
	std::unique_ptr<Object3d> injectionLeftObj_;
	std::unique_ptr<Object3d> injectionRightObj_;

	Vector3 injectionLeftPos_{ -2.5f,1.0f,-1.5f };
	Vector3 injectionRightPos_{ 2.5,1.0f,-1.5f };


	int index_b = 0;
	int maxBullet = 1;
	int bulletNum = 0;

private:

};

