#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include "DirectXGame/engine/collider/3d/Collider.h"
#include "DirectXGame/engine/Collider/ContactRecord.h"

class FollowCamera;
class DirectXCommon;
class Entity3DManager;
class Entity2DManager;
class BulletManager;
class BasePlayer : public Collider
{
public:
	struct Parameters {
		float HP;			// HP
		float MaxHP;		// HPMAX
		float moveSpeed;	// 移動速度
	};

	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	///< summary>
	/// 描画
	///</summary>
	virtual void Draw() = 0;

	virtual void DrawP()= 0;

	virtual void Draw2D()= 0;
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

	Object3d* GetObject3D() { return object_.get(); }

	bool GetInvincible() const { return isInvincible_; }


public:
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;

	virtual Vector3 GetCenterPosition() const;

protected:
	// オブジェクト
	std::unique_ptr<Object3d> object_ = std::make_unique<Object3d>();

	
	ContactRecord contactRecord_;
	// シリアルナンバー
	uint32_t serialNumber = 0;

	bool isAlive_ = true;
	bool isInvincible_ = false;

	Parameters parameter_;
	// 移動
	Vector3 velocity_;

	
protected:
	FollowCamera* followCamera_;
	Entity3DManager* entity3DManager_;
	Entity2DManager* entity2DManager_;
	Camera* camera_ = nullptr;
private:

};

