#pragma once
#include "DirectXGame/application/base/BaseClass/Special/BaseSpecial.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

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
	void InAction();


	void SetReticleParent(WorldTransform* parent) { objectReticle_->GetWorldTransform().parent_ = parent; };

public:

	// 描画するか
	void SetIsDraw(bool is) { objectReticle_->SetIsDraw(is); };

	void SetStage(Stage* stage);

	


	void SetRadius(float rad) { reticleRad_ = rad; }

	// 半径爆心
	float GetRadius() const { return reticleRad_; };

	// 爆心位置 
	Vector3 GetRangeBombingPos() const { return rangeBombingPos; }

	void Set(FollowCamera* followCamera, BulletManager* bulletManager) 
	{
		this->followCamera = followCamera;
		this->bulletManager = bulletManager;
	}

private:
	Object3d* objectReticle_;				// オブジェクトレティクル

	int index_b = 0;
	int maxBullet = 1;
	int bulletNum = 0;
	int currentMissileIndex = 0;

	float shotTimer = 0.0f;

	float reticleRad_ = 100.0f;				// レティクルの半径　　　
	Vector3 rangeBombingPos{};				// レンジボムの位置
private:
	Stage* stage_ = nullptr;
	FollowCamera* followCamera = nullptr;
	BulletManager* bulletManager = nullptr;
};

