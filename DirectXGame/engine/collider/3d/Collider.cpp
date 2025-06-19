#include "Collider.h"

#pragma region 球
void SphereCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
    centerWorld = worldTransform.worldMat_.GetWorldPosition();

#ifdef _DEBUG
    if (lineCommon) {
        lineCommon->AddLineSphere({ centerWorld ,radius });
    }
#endif // _DEBUG


}

bool SphereCollider::CheckHit(const Collider& other) const
{
    if (!other.enabled) return false;

    // 球
    if (other.GetType() == ColliderType::Sphere) {
        auto& o = static_cast<const SphereCollider&>(other);
        float distSq = (centerWorld - o.centerWorld).LengthSq();
        float radiusSum = radius + o.radius;
        return distSq <= radiusSum * radiusSum;
    }

    // AABB
    if (other.GetType() == ColliderType::AABB) {
        auto& o = static_cast<const AABBCollider&>(other);
        return IsCollision(AABB(o.minWorld, o.maxWorld), Sphere{ centerWorld ,radius });
    }

    // カプセル
    if (other.GetType() == ColliderType::Capsule) {
        auto& o = static_cast<const CapsuleCollider&>(other);
        return false;
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);
        return false;
    }

    // AABBとの衝突など他の型は別で判定
    return false;
}

bool SphereCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const
{
    if (other.GetType() != ColliderType::Sphere) {
        return false; // 他の形状は未対応
    }

    const SphereCollider& o = static_cast<const SphereCollider&>(other);
    Vector3 diff = centerWorld - o.centerWorld;
    float dist = diff.Length();

    float radiusSum = radius + o.radius;

    if (dist < radiusSum && dist > 0.0001f) {
        float pushDepth = radiusSum - dist;
        Vector3 pushDir = diff / dist; // Normalizeと同じだが安全
        outPushVec = pushDir * pushDepth;
        return true;
    }

    return false;
}
#pragma endregion 

#pragma region AABB
void AABBCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
    centerWorld = worldTransform.worldMat_.GetWorldPosition();
    minWorld = aabb.min_ + centerWorld;
    maxWorld = aabb.max_ + centerWorld;


#ifdef _DEBUG
    if (lineCommon) {
        lineCommon->AddLineAABB(aabb, centerWorld);
    }
#endif // _DEBUG

}

bool AABBCollider::CheckHit(const Collider& other) const
{
    if (!other.enabled) return false;

    // 球
    if (other.GetType() == ColliderType::Sphere) {
        auto& o = static_cast<const SphereCollider&>(other);
        return IsCollision(AABB(minWorld, maxWorld), Sphere{ o.centerWorld ,o.radius });
    }

    // AABB
    if (other.GetType() == ColliderType::AABB) {
        auto& o = static_cast<const AABBCollider&>(other);
        return (minWorld.x <= o.maxWorld.x && maxWorld.x >= o.minWorld.x) &&
            (minWorld.y <= o.maxWorld.y && maxWorld.y >= o.minWorld.y) &&
            (minWorld.z <= o.maxWorld.z && maxWorld.z >= o.minWorld.z);
    }

    // カプセル
    if (other.GetType() == ColliderType::Capsule) {
        auto& o = static_cast<const CapsuleCollider&>(other);
        return false;
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);
        return false;
    }

    return false;
}

bool AABBCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
    if (!other.enabled) return false;

    return false;
}
#pragma endregion

#pragma region Capsule
void CapsuleCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
    centerWorld = worldTransform.worldMat_.GetWorldPosition();
   
    Capsule capWorld{ Vector3{capsule.segment.origin + centerWorld},Vector3{capsule.segment.end + centerWorld},{capsule.radius}};

#ifdef _DEBUG
    if (lineCommon) {
        lineCommon->AddLineCapsule(capWorld);
    }
#endif // _DEBUG

}

bool CapsuleCollider::CheckHit(const Collider& other) const
{
    if (!other.enabled) return false;


    // 球
    if (other.GetType() == ColliderType::Sphere) {
        auto& o = static_cast<const SphereCollider&>(other);
        return false;
    }

    // AABB
    if (other.GetType() == ColliderType::AABB) {
        auto& o = static_cast<const AABBCollider&>(other);
        return false;
    }

    // カプセル
    if (other.GetType() == ColliderType::Capsule) {
        auto& o = static_cast<const CapsuleCollider&>(other);
        return false;
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);
        return false;
    }
    
  
    return false;
}

bool CapsuleCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
    if (!other.enabled) return false;

    return false;
}
#pragma endregion

#pragma region OBB
void OBBCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
    centerWorld = worldTransform.worldMat_.GetWorldPosition();

    obb.center = centerWorld;
#ifdef _DEBUG
    if (lineCommon) {
        lineCommon->AddLineOBB(obb);
    }
#endif // _DEBUG

}

bool OBBCollider::CheckHit(const Collider& other) const
{
    if (!other.enabled) return false;

    // 球
    if (other.GetType() == ColliderType::Sphere) {
        auto& o = static_cast<const SphereCollider&>(other);
        return false;
    }

    // AABB
    if (other.GetType() == ColliderType::AABB) {
        auto& o = static_cast<const AABBCollider&>(other);
        return false;
    }

    // カプセル
    if (other.GetType() == ColliderType::Capsule) {
        auto& o = static_cast<const CapsuleCollider&>(other);
        return false;
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);
        return false;
    }

    return false;
}

bool OBBCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
    if (!other.enabled) return false;

    return false;
}
#pragma endregion



