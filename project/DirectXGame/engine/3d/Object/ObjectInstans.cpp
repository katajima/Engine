#include "ObjectInstans.h"
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>
#include "DirectXGame/engine/Move/RigidBodyComponent.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/MyGame/MyGame.h"

#pragma region ObjectInstans

Engine::ObjectInstans::ObjectInstans() = default;

Engine::ObjectInstans::~ObjectInstans() = default;

Engine::ObjectInstans::ObjectInstans(ObjectInstans&&) noexcept = default;
Engine::ObjectInstans& Engine::ObjectInstans::operator=(ObjectInstans&&) noexcept = default;

void Engine::ObjectInstans::Initialize(EntityManager* entity3DManager, bool useCollider, bool rigidUpdate, Transform transfor) {
	transform.Initialize();
	transform.translate_ = transfor.translate;
	transform.rotate_ = transfor.rotate;
	transform.scale_ = transfor.scale;
	color = { 1,1,1,1 };
	useCollider_ = useCollider;
	rigidUpdate_ = rigidUpdate;
	isDelete_ = false;
	is_ = false;

	if (useCollider_) {
		colliderComponent_ = std::make_unique<ColliderComponent>();
		colliderComponent_->SetOwner(colliderComponent_.get());
		// ラインコモンをセット
		colliderComponent_->SetLineCommon(entity3DManager->Get3DLineCommon());
		// 登録（IDを取得したければ変数で受ける）
		colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());
		isColliderComponenyUpdate_ = true;
	}

	rigidBodyComponent_ = std::make_unique<RigidBodyComponent>();

}

void Engine::ObjectInstans::Update() {
	if (isDelete_) return;
	transform.Update();
	// コライダー
	if (GetColliderComponent()) {
		if (isColliderComponenyUpdate_) {
			colliderComponent_->UpdateAll(transform);
		}
	}

	// 物理
	if (GetRigidBodyComponent() && rigidUpdate_) {
		rigidBodyComponent_->Integrate(MyGame::GameTime(), transform);
	}
}


Engine::ContactRecord& Engine::ObjectInstans::GetContactRecord() { return colliderComponent_->contactRecord_; };
// Object3d内でコライダーコンポーネントを更新するか
void Engine::ObjectInstans::SetIsUpdateColliderComponent(bool is) { isColliderComponenyUpdate_ = is; };
// コライダーコンポーネントを取得
Engine::ColliderComponent* Engine::ObjectInstans::GetColliderComponent() { return colliderComponent_.get(); };
// リジットボディー取得
Engine::RigidBodyComponent* Engine::ObjectInstans::GetRigidBodyComponent() { return rigidBodyComponent_.get(); };

#pragma endregion