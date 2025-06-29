#pragma once
#include "DirectXGame/engine/Effect/Particle/ParticleEmitter.h"
#include "DirectXGame/engine/Effect/Particle/ParticleManager.h"

#include "DirectXGame/application/base/Character/BaseCharacter.h"

class Player;
class Entity3DManager;
class Entity2DManager;
class BaseEnemy : public BaseCharacter {
public:
	Matrix4x4 nullChek;
public:
	// デフォルトコンストラクタ
	BaseEnemy();

	// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;


	// 毎フレーム更新
	virtual void Update() = 0;

	virtual void DrawEffect() = 0;

	virtual void Draw2D() = 0;

	virtual void SetPlayer(Player* player) = 0;

public:
	// パーティクル発生
	virtual void Emit() = 0;
	
	// ダメージ
	void AddDamage(float damage) {
		HP() -= damage;
		if (GetHP() <= 0) {
			HP() = 0;
			Situations().isAlive = false; // 敵が死亡
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

	void SetID(uint32_t id) { id_ = id; }

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

protected:
	uint32_t id_ = 0; // ID

	// オブジェクト
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

	// ロックオンされているか
	bool isLockOn = false;
protected: //2D
	std::unique_ptr<Sprite> icon_lockOn;
	std::unique_ptr<Sprite> hpBer_;
	std::unique_ptr<Sprite> backHpBer_;
protected:
	Player* player_;
};
