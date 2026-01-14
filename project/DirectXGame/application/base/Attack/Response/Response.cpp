#include "Response.h"
#include <DirectXGame/application/base/Object/ObjectComponent.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>



#pragma region ResponseSystem


void ResponseSystem::Initialize(CharacterParameterComponent* paremeter, ObjectComponent* object) {

	// 攻撃衝突応答クラス初期化
	attackResponse_ = std::make_unique<AttackResponse>();
	attackResponse_->Initialize(paremeter,object);

	// 接触衝突応答クラス
	hitResponse_ = std::make_unique<HitResponse>();
	hitResponse_->SetOwner(&object->GetWorldTransform());

}

void ResponseSystem::Update(float dt) {
	attackResponse_->Update(dt);
}

#pragma endregion // 応答システム


#pragma region AttackResponse

void AttackResponse::Initialize(CharacterParameterComponent* paremeter, ObjectComponent* object) {
	assert(paremeter && object);
	this->paremeter = paremeter;
	this->object = object;

	// 攻撃衝突モーションクラス初期化
	hitMotionSystem_ = std::make_unique<HitMotionSystem>();
	
}

void AttackResponse::Update(float dt) {
	// 被撃モーション更新
	hitMotionSystem_->Update(dt,object, paremeter);
}

#pragma endregion // 攻撃を受けたとき


#pragma region HitResponse

void HitResponse::Hit(CollisionTag tag, Engine::Collider* self, Engine::Collider* other)
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
				transform->translate_ += pushVec * halfSize;
			}

			transform->Update();
		}
	}
}

#pragma endregion	// 衝突応答

