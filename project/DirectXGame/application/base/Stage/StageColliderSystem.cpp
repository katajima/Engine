#include "StageColliderSystem.h"

void StageColliderSystem::Initialize(Engine::LineCommon* lineCommon) {
	colliderComponent_ = std::make_unique<Engine::ColliderComponent>();
	colliderComponent_->SetLineCommon(lineCommon);
	colliderComponent_->SetOwner(this);
	GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
		if (!otherComponent) return;
		};
}

void StageColliderSystem::CreateCollider() {

	for (auto& obj : objects) {
		for (auto& mesh : obj->GetModel()->GetModelData().mesh) {
			for (auto& triangle : mesh->triangle) {
				auto triangleColl = std::make_unique<Engine::TriangleCollider>();
				triangleColl->triangle01 = triangle.vertices[0];
				triangleColl->triangle02 = triangle.vertices[1];
				triangleColl->triangle03 = triangle.vertices[2];
				triangleColl->tag = CollisionTag::Wall;
				triangleColl->layer = CollisionLayer::Environment;
				triangleColl->collisionMask = 0xFFFFFFFF;
				triangleColl->isStatic = true;
				triangleColl->Enable();
				colliderComponent_->AddCollider(std::move(triangleColl));
			}
		}
	}
	
}

void StageColliderSystem::Update() {

	colliderComponent_->UpdateAll(objects[0]->GetWorldTransform());
};
