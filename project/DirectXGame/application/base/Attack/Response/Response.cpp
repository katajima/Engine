#include "Response.h"
#include <DirectXGame/application/base/Object/ObjectComponent.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>

#pragma region HitResponse

void HitResponse::Hit(CollisionTag tag, Engine::Collider* self, Engine::Collider* other)
{
	if (other->tag == tag) {
		Vector3 pushVec{};
		if (self->ResolveCollision(*other, pushVec)) {
			pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）

			// リアクション移動システムがあるなら
			if (moveRequestSystem) {
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
				moveRequestSystem->SetRequest(request);
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
			if (moveRequestSystem) {
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
				moveRequestSystem->SetRequest(request);
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
		if (moveRequestSystem) {
			MoveRequest request;
			request.velocity = dire * 20.0f;
			moveRequestSystem->SetRequest(request);
			return;
		}
	}
}

#pragma endregion	// 衝突応答

