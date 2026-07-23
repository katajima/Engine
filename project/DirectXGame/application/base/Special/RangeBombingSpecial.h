#pragma once
#include "DirectXGame/application/base/Special/Base/BaseSpecial.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

// 前方宣言
class MainStage;
namespace Engine {
	class EntityManager;
	class Object3d;
	class CylinderPrimitive;
}
/// <summary>
/// 範囲攻撃必殺技
/// </summary>
class RangeBombingSpecial : public BaseSpecial
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Engine::EntityManager* entityManager, Engine::Camera* camera) override;

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

	/// <summary>
	/// レティクル親子付け
	/// </summary>
	void SetReticleParent(Engine::WorldTransform* parent);

public:
	/// <summary>
	/// 描画するか
	/// </summary>
	void SetIsDraw(bool is);
	
	/// <summary>
	/// 半径設定
	/// </summary>
	void SetRadius(float rad);

	/// <summary>
	/// 半径爆心
	/// </summary>
	float GetRadius() const { return dataRange_.reticleRad_; };

	/// <summary>
	/// 爆心位置
	/// </summary>
	Vector3 GetRangeBombingPos() const { return dataRange_.rangeBombingPos; }

	/// <summary>
	/// フォローカメラと弾マネージャー設定
	/// </summary>
	void Set(FollowCamera* followCamera, BulletSpawn* bulletSpawn){
		this->followCamera = followCamera;
		this->spawn = bulletSpawn;
	}
private:
	FollowCamera* followCamera = nullptr;
	BulletSpawn* spawn = nullptr;
	MainStage* stage = nullptr;
private:
	// ミサイルオブジェクト
	std::vector<Engine::Object3d*> missiles_;

	Engine::Object3d* objectReticle_ = nullptr;				// オブジェクトレティクル
	std::unique_ptr<Engine::CylinderPrimitive> cylinder_ = nullptr;
private:



	struct Data {
		int maxBullet = 1;
		int bulletNum = 0;
		int currentMissileIndex = 0;

		float shotTimer = 0.0f;

		float reticleRad_ = 30.0f;				// レティクルの半径　　　
		Vector3 rangeBombingPos{};				// レンジボムの位置
	};
	Data dataRange_;


private: // 一旦

	// 弾発射に使うインターバル（秒）
	const float fireInterval = 0.01f; // 例：0.04秒ごとに1発ずつ発射

	struct ProvisionalData {
		Vector3 translate = { 0,2,0 };
		Vector3 rotate = { Math::DegreesToRadians(-90), 0, 0 };


		int maxGauge_ = 20;			// 最大ゲージ設定
		int maxBullet = 40;			// 発射数
		int bulletNum = 0;			// 弾番号

		int cylinderSegments = 32;
		float cylinderHeight = 5.0f;

		float shotTimer = 0.5f;

		/// <summary>
		/// ミサイルの配置データ
		/// </summary>

		int missileNumX = 5;
		int missileNumY = 2;

		Vector3 missileTranslate = { 3500,106,3000 };
		Vector2 missileInterval = { 120.0f,100.0f };
		Vector3 missileRotate = { 0 ,Math::DegreesToRadians(-90),0 };
		float  missileSize = 10.0f;
	};
	ProvisionalData provisionalData_;
};

