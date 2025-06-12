#include "CollisionManager.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
void CollisionManager::Reset() 
{
	// リストを空っぽにする
	colliders_.clear();
}

CollisionManager::~CollisionManager()
{


}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) 
{
	// 球同士
	if (colliderA->GetColliderType() == static_cast<uint32_t>(ColliderType::Sphere) &&
		colliderB->GetColliderType() == static_cast<uint32_t>(ColliderType::Sphere)) {
		CollFanction::SphereAndSphere(colliderA, colliderB);
	}

	// 
	if (colliderA->GetColliderType() == static_cast<uint32_t>(ColliderType::Capsule) &&
		colliderB->GetColliderType() == static_cast<uint32_t>(ColliderType::Capsule)) {
		CollFanction::CapsuleAndCapsule(colliderA, colliderB);
	}
	
}

void CollisionManager::CheckAllCollisions()
{
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;
		// イテレータBはi\イテレータAの次の要素から回す(重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::AddCollider(Collider* collider) 
{
	// NULLポインタでないことを確認
	if (collider != nullptr) {
		colliders_.push_back(collider);
	}
}

void CollisionManager::Initialize(GlobalVariables* globalVariables) 
{


	globalVariables_ = globalVariables;
	
	const char* gropName = "CollisionManager";
	// グループを追加する
	globalVariables_->CreateGroup(gropName);
	globalVariables_->AddItem(gropName, "isCollider", isCollider);
}

void CollisionManager::UpdateWorldTransform(LineCommon* lineCommon)
{

	ApplyGlobalVariables();

	// 非表示なら抜ける
	//if (isCollider == false) {
	//	//return;
	//}

	// 全てのコライダーについて
	for (Collider* collider : colliders_) {
		//更新
		collider->UpdateWorldTransform(lineCommon);
	}

}

void CollisionManager::Draw()
{

	//// 非表示なら抜ける
	//if (isCollider == false) {
	//	return;
	//}

	//// 全てのコライダーについて
	//for (Collider* collider : colliders_) {
	//	// 更新
	//	//collider->Draw();
	//}

}

void CollisionManager::ApplyGlobalVariables()
{
	const char* gropName = "CollisionManager";
	// グループを追加する
	globalVariables_->CreateGroup(gropName);
	isCollider = globalVariables_->GetBoolValue(gropName, "isCollider");

}
