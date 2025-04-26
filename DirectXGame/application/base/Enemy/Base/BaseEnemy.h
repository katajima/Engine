#pragma once
#include "DirectXGame/engine/collider/3d/Collider.h"
#include"DirectXGame/engine/collider/ContactRecord.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"


class Player;
class Entity3DManager;
class Entity2DManager;
class BaseEnemy : public Collider {
public:

	struct Parameters {
		float HP;			// HP
		float MaxHP;		// HPMAX
		float moveSpeed;	// 移動速度
		float damege;		// ダメージ

	};


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

protected:
	// 時間
	float Timer() const;

	void Shake();

	void HitStpoTime();

	// 
	void HitMotion();

public: // コライダー関係
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;

	virtual Vector3 GetCenterPosition() const;
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

protected:
	std::unique_ptr<Sprite> icon_lockOn;
	std::unique_ptr<Sprite> hpBer_;
	std::unique_ptr<Sprite> backHpBer_;
protected:
	Player* player_;
	Entity3DManager* entity3DManager_;
	Entity2DManager* entity2DManager_;

};
