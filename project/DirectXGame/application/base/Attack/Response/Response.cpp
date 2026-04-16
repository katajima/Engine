#include "Response.h"
#include <DirectXGame/application/base/Object/ObjectComponent.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>



#pragma region ResponseSystem


void ResponseSystem::Initialize(Character::BaseCharacter* owner) {
	this->owner = owner;
	// 攻撃衝突応答クラス初期化
	attackResponse_ = std::make_unique<AttackResponse>();
	attackResponse_->Initialize(owner);

	// 接触衝突応答クラス
	hitResponse_ = std::make_unique<HitResponse>();
	hitResponse_->SetOwner(&owner->GetWorldTransform());

}

void ResponseSystem::Update(float dt) {
	attackResponse_->Update(dt);
}

#pragma endregion // 応答システム


#pragma region AttackResponse

void AttackResponse::Initialize(Character::BaseCharacter* owner) {
	
	// 攻撃衝突モーションクラス初期化
	hitMotionSystem_ = std::make_unique<HitMotionSystem>();
	hitMotionSystem_->Initialize(owner);
}

void AttackResponse::Update(float dt) {
	// 被撃モーション更新
	hitMotionSystem_->Update(dt);
}

#pragma endregion // 攻撃を受けたとき


#pragma region HitResponse

void HitResponse::Hit(CollisionTag tag, Engine::Collider* self, Engine::Collider* other)
{
	if (other->tag == tag) {
		Vector3 pushVec{};
		if (self->ResolveCollision(*other, pushVec)) {
			pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）

			// リアクション移動システムがあるなら
			if (responseMoveSystem) {
				MoveRequest request;
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					request.velocity = pushVec;
				}
				else if (self->isStatic) {
					// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					request.velocity = pushVec * halfSize;
				}
				responseMoveSystem->SetRequest(request);
				return;
			}
		}
	}
}

void HitResponse::HitWall(Engine::Collider* self, Engine::Collider* other) {
	if (other->tag == CollisionTag::Wall) {
		Vector3 pushVec{};
		if (self->ResolveCollision(*other, pushVec)) {
			// リアクション移動システムがあるなら
			if (responseMoveSystem) {
				MoveRequest request;
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					request.velocity = pushVec;
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					request.velocity = pushVec * halfSize;
				}
				if (other->tag == CollisionTag::Wall) {
					if (pushVec.Normalize().y >= 0.7f) {
						request.isLanding = true;
					}
					else if (pushVec.Length() == 0.0f) {
						request.isLanding = true;
					}
				}

				request.groundHeight = request.velocity.y + other->centerWorld.y;
				responseMoveSystem->SetRequest(request);
				return;
			}
		}
	}
}

void HitResponse::HitEffect(Engine::Collider* self, Engine::Collider* other) {
	if (other->tag == CollisionTag::PlayerEffect) {
		Vector3 pushVec{};
		Vector3 otherPos = other->centerWorld;
		Vector3 selfPos = self->centerWorld;
		Vector3 dire = Normalize(otherPos - selfPos);

		// リアクション移動システムがあるなら
		if (responseMoveSystem) {
			MoveRequest request;
			request.velocity = dire * 20.0f;
			responseMoveSystem->SetRequest(request);
			return;
		}
	}
}

#pragma endregion	// 衝突応答

