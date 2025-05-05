#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include "DirectXGame/engine/collider/3d/Collider.h"
#include "DirectXGame/engine/Collider/ContactRecord.h"

class Entity3DManager;
class BaseWeapon : public Collider
{
public:
	struct Parameters {
		
		float moveSpeed;	// 移動速度
	};


	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Entity3DManager* entity3DManager, Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	///< summary>
	/// 描画
	///</summary>
  	virtual void Draw()= 0;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;

	virtual Vector3 GetCenterPosition() const;



public:

	Object3d* GetObject3D() { return object_.get(); }

	void SetOffset(const Vector3& translation) { offset = translation; }

	// 接触履歴を抹消
	void ContactRecordClear();

	void SetRad(float rad) { Collider::SetRadius(rad); };


protected:

	virtual void UsePlayer() = 0;

	virtual void UseEnemy() = 0;



protected:
	// オブジェクト
	std::unique_ptr<Object3d> object_ = std::make_unique<Object3d>();
	const Camera* camera_ = nullptr;


	ContactRecord contactRecord_;
	Vector3 offset;
private:


};