#pragma once

#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"

#include "DirectXGame/engine/Effect/Particle/ParticleEmitter.h"
#include "DirectXGame/engine/Effect/Particle/ParticleManager.h"

#include "DirectXGame/engine/collider/3d/ColliderComponent.h"
#include "DirectXGame/engine/Base/pch.h"

class BasePlayer;
class BaseEnemy;
class Entity3DManager;
class Entity2DManager;
class BaseBullet{
public:
	// デフォルトコンストラクタ
	BaseBullet() = default;
	virtual ~BaseBullet() = 0;
	
	struct Parameters {
		float HP;			// HP
		float MaxHP;		// HPMAX
		float moveSpeed;	// 移動速度
		float damege;		// ダメージ

	};
	
	// 初期化
	virtual void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera) = 0;


	// 毎フレーム更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw() = 0;
	//
	virtual void DrawP() = 0;

	virtual void Draw2D() = 0;
	
	virtual bool IsExpired() const {
		return !GetAlive() && !GetIsEffectPlay(); // ← 演出含めて完全終了
	}
public:
	// 生存判定
	bool GetAlive() const { return isAlive_; }
	//
	bool GetIsEffectPlay() const { return isEffectPlay_; }

	// ダメージ
	void AddDamage(float damage) {
		parameter_.HP -= damage;
		if (parameter_.HP <= 0) {
			parameter_.HP = 0;
			isAlive_ = false; // 敵が死亡
		}
	}
	// 時間
	float GetTimer() const;

	// オブジェクト
	Object3d* GetObject3D() { return object_; }
	//
	void SetPlayer(BasePlayer* player);

	void SetEnemy(BaseEnemy* enemy);

	void SetTargerRange(Vector3 pos, float rad) { targetRange_ = { pos,rad }; };

	ColliderComponent* GetColliderComponent() { return object_->GetColliderComponent(); }

protected:
	// 当たり判定をするか
	void SetIsCollision(bool is) { isCollision = is; }
public:
	
protected:
	Object3d* object_;
	
	// 各パラメータ
	Parameters parameter_{};
	// 移動
	Vector3 velocity_{};
	// デスフラグ
	bool isAlive_ = false;
	// effect
	bool isEffectPlay_ = false;

	// ヒット
	bool Hit = false;
	// 
	bool isCollision = true;

	struct TargerRange {
		Vector3 position; // 位置
		float radius;    // 半径
	} targetRange_;

protected:
	BasePlayer* player_;
	BaseEnemy* enemy_;
	Entity3DManager* entity3DManager_;
	Entity2DManager* entity2DManager_;
};


