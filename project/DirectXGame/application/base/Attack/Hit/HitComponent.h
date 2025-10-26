#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/math/random.h"
#include "DirectXGame/application/base/Attack/AttackData.h"


/// <summary>
/// ヒットコンポーネント
/// </summary>
class HitMotionComponent 
{
public:

	// 初期化
	void Init(float hitTimer,Vector3 offset) {
		maxHitStopTimer = hitTimer;
		offset_ = offset;
	}

	// 更新
	void Update(float deltaTime, ObjectComponent* object) {

		if (isHit) {
			object->GetRigidBodyComponent()->SetIsGravity(false);

			//object->GetWorldTransform().translate_.x = strPos_.x + Random::RandomFloat(-offset_.x, offset_.x);
			//object->GetWorldTransform().translate_.y = strPos_.y + Random::RandomFloat(-offset_.y, offset_.y);
			//object->GetWorldTransform().translate_.z = strPos_.z + Random::RandomFloat(-offset_.z, offset_.z);


			hitStopTimer += deltaTime;
			if (maxHitStopTimer > hitStopTimer) {
				isHit = false;
				hitStopTimer = 0.0f;
				knockbackTimer = 0.0f;;
			}
			
		}
		else {
			

			strPos_ = object->GetWorldTransform().GetWorldPosition();


			knockbackTimer += deltaTime;
			if (isknockback && knockbackTimer <= maxKnockbackTimer) {
				object->GetWorldTransform().translate_ += knockback_.DirectionPower() * deltaTime;
			}
			else {
				object->GetRigidBodyComponent()->SetIsGravity(true);
			}
			
		}

	}

	// ヒットしたか設定
	void SetIsHit(bool is) { isHit = is; }
	// ノックバックデータセット
	void SetKnockbackData(KnockbackData data) { knockback_ = data; }
	// ノックバックデータ
	KnockbackData GetKnockbackData() const { return knockback_; }
	// ノックバックするか
	void SetIsKnockback(bool is) { isknockback = is; }
	// ヒットストップ持続時間設定
	void SethitStopTime(float time) { maxHitStopTimer = time; };
	// ノックバック持続時間設定
	void SetKnockbackTime(float time) { maxKnockbackTimer = time; };


private:
	// ヒットストップ
	float hitStopTimer = 0.0f;
	float maxHitStopTimer = 0.5f;
	
	float knockbackTimer = 0.0f;
	float maxKnockbackTimer = 1.0f;

	// ヒット
	bool isHit = false;
	bool isknockback = false;

	// 初期位置
	Vector3 strPos_;
	// 揺れ
	Vector3 offset_{};
	// ノックバックデータ
	KnockbackData knockback_;
};
