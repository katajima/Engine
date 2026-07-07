#include "ObjectInstance.h"
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>
#include "DirectXGame/engine/Move/RigidBodyComponent.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/MyGame/MyGame.h"

#pragma region ObjectInstance

Engine::ObjectInstance::ObjectInstance() = default;

Engine::ObjectInstance::~ObjectInstance() = default;

Engine::ObjectInstance::ObjectInstance(ObjectInstance&&) noexcept = default;
Engine::ObjectInstance& Engine::ObjectInstance::operator=(ObjectInstance&&) noexcept = default;

void Engine::ObjectInstance::Initialize(EntityManager* entity3DManager, bool useCollider, bool rigidUpdate, Transform transfor) {
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

void Engine::ObjectInstance::Update() {
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


Engine::ContactRecord& Engine::ObjectInstance::GetContactRecord() { return colliderComponent_->contactRecord_; };
// Object3d内でコライダーコンポーネントを更新するか
void Engine::ObjectInstance::SetIsUpdateColliderComponent(bool is) { isColliderComponenyUpdate_ = is; };
// コライダーコンポーネントを取得
Engine::ColliderComponent* Engine::ObjectInstance::GetColliderComponent() { return colliderComponent_.get(); };
// リジットボディー取得
Engine::RigidBodyComponent* Engine::ObjectInstance::GetRigidBodyComponent() { return rigidBodyComponent_.get(); };

#pragma endregion