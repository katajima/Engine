#include "ColliderComponent.h"

using namespace Engine;

void ColliderComponent::UpdateAll(const WorldTransform& worldTransform)
{
    // 更新
    for (auto& entry : colliders) {
        entry.collider->Update(worldTransform, lineCommon);
    }
}

void ColliderComponent::UpdateByTag(const WorldTransform& worldTransform, CollisionTag tag)
{
    // タグでの更新
    for (auto& entry : colliders) {
        if (entry.collider->tag == tag) {
            entry.collider->Update(worldTransform, lineCommon);
        }
    }
}

void ColliderComponent::UpdateByID(const WorldTransform& worldTransform, uint32_t id)
{
    // IDでの更新
    for (auto& entry : colliders) {
        if (entry.id == id) {
            entry.collider->Update(worldTransform, lineCommon);
        }
    }
}

void ColliderComponent::CheckAndNotify(ColliderComponent& other)
{
    for (const auto& c1 : colliders) {
        for (const auto& c2 : other.colliders) {
            if (c1.collider->CheckHit(*c2.collider)) {
                if (onHitCallback) {
                    onHitCallback(c1.collider.get(), c2.collider.get());
                }
                if (other.onHitCallback) {
                    other.onHitCallback(c2.collider.get(), c1.collider.get());
                }
            }
        }
    }
}

uint32_t ColliderComponent::AddCollider(std::unique_ptr<Collider> collider) {
    collider->owner = owner;
    collider->id = nextId_;
    uint32_t id = nextId_++;
    colliders.push_back({ id, std::move(collider) });
    return id;
}


void ColliderComponent::SetOwner(void* newOwner){
    owner = newOwner;
    for (auto& entry : colliders) {
        entry.collider->owner = newOwner;
    }
}

void ColliderComponent::SetEnableByTag(CollisionTag tag, bool enable) {
    for (auto& entry : colliders) {
        if (entry.collider->tag == tag) {
            entry.collider->enabled = enable;
        }
    }
}

void ColliderComponent::SetEnableById(uint32_t id, bool enable) {
    for (auto& entry : colliders) {
        if (entry.id == id) {
            entry.collider->enabled = enable;
        }
    }
}