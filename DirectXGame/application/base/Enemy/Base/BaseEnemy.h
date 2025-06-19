#pragma once
#include "DirectXGame/engine/collider/3d/Collider.h"
#include "DirectXGame/engine/collider/3d/ColliderComponent.h"
#include"DirectXGame/engine/collider/ContactRecord.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"

#include "DirectXGame/engine/Effect/Particle/ParticleEmitter.h"
#include "DirectXGame/engine/Effect/Particle/ParticleManager.h"


class Player;
class Entity3DManager;
class Entity2DManager;
class BaseEnemy{
public:

	struct Parameters {
		float HP;			// HP
		float MaxHP;		// HPMAX
		float moveSpeed;	// 移動速度
		float damege;		// ダメージ

	};

	Matrix4x4 nullChek;
public:
	// デフォルトコンストラクタ
	BaseEnemy();

	// 初期化
	virtual void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera) = 0;


	// 毎フレーム更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw() = 0;
	//
	virtual void DrawP() = 0;

	virtual void Draw2D() = 0;

	virtual void SetPlayer(Player* player) = 0;

public:
	// パーティクル発生
	virtual void Emit() = 0;
	// 生存判定
	bool GetAlive() const { return isAlive_; }
	// ダメージ
	void AddDamage(float damage) {
		parameter_.HP -= damage;
		if (parameter_.HP <= 0) {
			parameter_.HP = 0;
			isAlive_ = false; // 敵が死亡
		}
	}
	// ロックオンされているか
	bool GetLockOn() const { return isLockOn; }
	// 
	void SetLockOn(bool lock) { isLockOn = lock; }
	// ヒットストップ
	void hitStop(float time) {
		hitStopTimer = time;
		kHitStopTimer = time;
	};
	// ヒットした
	void SetHit() { hit = true; };


	void SetSerialNumber(uint32_t num) { serialNumber = num; };
	// シリアルナンバー
	uint32_t GetSerialNumber() const { return serialNumber; }

	Object3d* GetObject3D() { return object_.get(); }

	ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); };

protected:
	// 時間
	float Timer() const;
	// シェイク
	void Shake();
	// ヒットストップ時間
	void HitStpoTime();
	// ヒットモーション
	void HitMotion();
	// スプライト初期化
	void Initialize2D();


public: // コライダー関係

protected:
	// オブジェクト
	std::unique_ptr<Object3d> object_ = std::make_unique<Object3d>();
	WorldTransform transBase_;
	Vector3 oldPos_;

	// 敵時間スピード
	float timeSpeed_ = 1.0f;
	// ヒットストップ
	float hitStopTimer;
	float kHitStopTimer;
	// ヒット
	bool hit = false;
	float count = 0;


	// パラメータ
	Parameters parameter_;

	// 移動
	Vector3 velocity_;

	// 生存フラグ
	bool isAlive_ = true;

	// ロックオンされているか
	bool isLockOn = false;


	// シリアルナンバー
	uint32_t serialNumber = 0;
	// 次のシリアルナンバー
	static uint32_t nextSerialNumber;
	ContactRecord contactRecord_;
	std::unique_ptr<ColliderComponent> colliderComponent_;

protected: //2D
	std::unique_ptr<Sprite> icon_lockOn;
	std::unique_ptr<Sprite> hpBer_;
	std::unique_ptr<Sprite> backHpBer_;
protected:
	Player* player_;
	Entity3DManager* entity3DManager_;
	Entity2DManager* entity2DManager_;

};
