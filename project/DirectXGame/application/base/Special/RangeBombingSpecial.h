#pragma once
#include "DirectXGame/application/base/Special/Base/BaseSpecial.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

// 前方宣言

namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
}
class BulletSpawn;
class FollowCamera;
class MainStage;

/// <summary>
/// 範囲攻撃必殺技
/// </summary>
class RangeBombingSpecial : public BaseSpecial
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::Camera* camera) override;

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
	void SetReticleParent(Engine::WorldTransform* parent) { objectReticle_->GetWorldTransform().parent_ = parent; };

public:
	

	// 描画するか
	void SetIsDraw(bool is) { objectReticle_->GetRenderComponent()->SetIsDraw(is); };
	// ステージ設定
	void SetStage(MainStage* stage) { stage_ = stage;};

	

	// 半径設定
	void SetRadius(float rad);

	// 半径爆心
	float GetRadius() const { return dataRange_.reticleRad_; };

	// 爆心位置 
	Vector3 GetRangeBombingPos() const { return dataRange_.rangeBombingPos; }

	// フォローカメラと弾マネージャー設定
	void Set(FollowCamera* followCamera, BulletSpawn* bulletSpawn){
		this->followCamera = followCamera;
		this->spawn = bulletSpawn;
	}

private:
	
private:
	Engine::Object3d* objectReticle_;				// オブジェクトレティクル

	std::unique_ptr<Engine::CylinderPrimitive> ctlinder_;

	struct Data {
		int maxBullet = 1;
		int bulletNum = 0;
		int currentMissileIndex = 0;

		float shotTimer = 0.0f;

		float reticleRad_ = 50.0f;				// レティクルの半径　　　
		Vector3 rangeBombingPos{};				// レンジボムの位置
	};
	Data dataRange_;
	
private:
	BulletSpawn* spawn = nullptr;			// 弾の出現

	MainStage* stage_ = nullptr;
	FollowCamera* followCamera = nullptr;
private: // 一旦

	// 弾発射に使うインターバル（秒）
	const float fireInterval = 0.01f; // 例：0.04秒ごとに1発ずつ発射

	struct ProvisionalData {
		Vector3 translate = { 0,2,50 };
		Vector3 rotate = { Math::DegreesToRadians(-90), 0, 0 };


		int maxGauge_ = 40;			// 最大ゲージ設定
		int maxBullet = 40;			// 発射数
		int bulletNum = 0;			// 弾番号

		int cylinderSegments = 32;
		float cylinderHeight = 5.0f;

		float shotTimer = 0.5f;
	};
	ProvisionalData provisionalData_;
};

