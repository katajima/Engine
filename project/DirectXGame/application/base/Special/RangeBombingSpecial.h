#pragma once
#include "DirectXGame/application/base/Special/Base/BaseSpecial.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"


// 前方宣言
class Entity3DManager;
class Entity2DManager;
class BulletManager;
class FollowCamera;
class BaseEnemy;
class Stage;

/// <summary>
/// 範囲攻撃必殺技
/// </summary>
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

	// レティクル親子付け
	void SetReticleParent(WorldTransform* parent) { objectReticle_->GetWorldTransform().parent_ = parent; };

public:

	// 描画するか
	void SetIsDraw(bool is) { objectReticle_->GetRenderComponent()->SetIsDraw(is); };
	// ステージ設定
	void SetStage(Stage* stage);

	

	// 半径設定
	void SetRadius(float rad) { reticleRad_ = rad; }

	// 半径爆心
	float GetRadius() const { return reticleRad_; };

	// 爆心位置 
	Vector3 GetRangeBombingPos() const { return rangeBombingPos; }

	// フォローカメラと弾マネージャー設定
	void Set(FollowCamera* followCamera, BulletManager* bulletManager) 
	{
		this->followCamera = followCamera;
		this->bulletManager = bulletManager;
	}

private:
	Object3d* objectReticle_;				// オブジェクトレティクル

	std::unique_ptr<CylinderPrimitive> ctlinder_;


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

