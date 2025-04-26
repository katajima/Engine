#pragma once
#include "DirectXGame/engine/collider/3d/Collider.h"
#include"DirectXGame/engine/collider/ContactRecord.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"


class Player;
class Entity3DManager;
class Entity2DManager;
class BaseEnemy  : public Collider {
public:
	
	struct Parameters {
		float HP_;			// HP
		float MaxHP_;		// HPMAX
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
	
	void SetPlayer(Player* player) { player_ = player; };
	
public: // コライダー関係
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;

	virtual Vector3 GetCenterPosition() const;
protected:
	// オブジェクト
	std::unique_ptr<Object3d> object_ = std::make_unique<Object3d>();

	// 敵時間スピード
	float timeSpeed_ = 1.0f;

	// パラメータ
	Parameters parameter_;

	// 生存フラグ
	bool isAlive_ = true;


	// シリアルナンバー
	uint32_t serialNumber = 0;
	// 次のシリアルナンバー
	static uint32_t nextSerialNumber;
	ContactRecord contactRecord_;

	Player* player_;
};
