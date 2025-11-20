#include "Response.h"
#include <DirectXGame/application/base/Object/ObjectComponent.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>

#pragma region AttackResponseData

void AttackResponseData::Initialize(CharacterParameterComponent* paremeter, ObjectComponent* object) {
	assert(paremeter && object);
	this->paremeter = paremeter;
	this->object = object;
}

void AttackResponseData::Update(float dt) {
	

	// 攻撃データ
	for (auto& data : datas_) {
		
	}

}

void AttackResponseData::SetHit(const AttackData& data) {
	datas_.push_back(data);
}

#pragma endregion // 攻撃を受けたとき


#pragma region HitResponse

void HitResponse::Hit(CollisionTag tag, Collider* self, Collider* other)
{
	if (other->tag == tag) {
		Vector3 pushVec;
		if (self->ResolveCollision(*other, pushVec)) {
			pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）
			if (other->isStatic) {
				// 相手が動かないなら自分だけ押し戻す
				transform->translate_ += pushVec;
			}
			else if (self->isStatic) {
				// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
			}
			else {
				// 双方が動く → 半分ずつ押し戻す（応用例）
				transform->translate_ += pushVec * 0.5f;
			}

			transform->Update();
		}
	}
}

#pragma endregion	// 衝突応答

