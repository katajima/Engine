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
		maxHitStopTimer = hitTimer;	// 最大ヒットストップ時間
		offset_ = offset;			// オフセット
	}

	// 更新
	void Update(float deltaTime, ObjectComponent* object) {

		// ヒットしたら
		if (isHit) {
			// 重力をオフに
			object->GetRigidBodyComponent()->SetIsGravity(false);

			
			hitStopTimer += deltaTime;
			if (maxHitStopTimer > hitStopTimer) {
				isHit = false;
				hitStopTimer = 0.0f;
				knockbackTimer = 0.0f;;
			}
			
		}
		else {
			
			// 初期位置
			strPos_ = object->GetWorldTransform().GetWorldPosition();

			// ノックバック時間更新
			knockbackTimer += deltaTime;
			if (isknockback && knockbackTimer <= maxKnockbackTimer) {
				// ノックバック方向にキャラクターを飛ばす
				object->GetWorldTransform().translate_ += knockback_.DirectionPower() * deltaTime;
			}
			else {
				// 重力をオンに
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
	
	// ノックバックタイマー
	float knockbackTimer = 0.0f;
	float maxKnockbackTimer = 1.0f; // 最大時間

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
