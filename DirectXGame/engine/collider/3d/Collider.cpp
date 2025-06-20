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
        return IsCollision(Sphere{ centerWorld ,radius },o.capWorld_);
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);
        return IsCollision(o.obb,Sphere{ centerWorld ,radius });
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

        return CapsuleIntersectsAABB(o.capWorld_, AABB(minWorld, maxWorld));
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);

        return IsCollision(o.obb, AABB(minWorld, maxWorld));
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
   
    capWorld_ = { Vector3{capsule.segment.origin + centerWorld},Vector3{capsule.segment.end + centerWorld},{capsule.radius}};
    
#ifdef _DEBUG
    if (lineCommon) {
        lineCommon->AddLineCapsule(capWorld_);
    }
#endif // _DEBUG

}

bool CapsuleCollider::CheckHit(const Collider& other) const
{
    if (!other.enabled) return false;


    // 球
    if (other.GetType() == ColliderType::Sphere) {
        auto& o = static_cast<const SphereCollider&>(other);
        return IsCollision(Sphere{ {o.centerWorld} ,{o.radius}}, capWorld_);
    }

    // AABB
    if (other.GetType() == ColliderType::AABB) {
        auto& o = static_cast<const AABBCollider&>(other);
        return CapsuleIntersectsAABB(capWorld_,AABB{o.minWorld,o.maxWorld});
    }

    // カプセル
    if (other.GetType() == ColliderType::Capsule) {
        auto& o = static_cast<const CapsuleCollider&>(other);
        return IsCollision(capWorld_,o.capWorld_);
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);
        return IsCollision(o.obb,capWorld_);
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
    obb.orientations[0] = worldTransform.worldMat_.Right().Normalize();
    obb.orientations[1] = worldTransform.worldMat_.Up().Normalize();
    obb.orientations[2] = worldTransform.worldMat_.Forward().Normalize();

#ifdef _DEBUG
    if (lineCommon) {
        lineCommon->AddLine(obb.center, obb.center + obb.orientations[0], { 1,0,0,1 }); // X軸: 赤
        lineCommon->AddLine(obb.center, obb.center + obb.orientations[1], { 0,1,0,1 }); // Y軸: 緑
        lineCommon->AddLine(obb.center, obb.center + obb.orientations[2], { 0,0,1,1 }); // Z軸: 青
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
        return IsCollision(obb,Sphere{o.centerWorld,o.radius });
    }

    // AABB
    if (other.GetType() == ColliderType::AABB) {
        auto& o = static_cast<const AABBCollider&>(other);
        return IsCollision(obb, AABB(o.minWorld, o.maxWorld));
    }

    // カプセル
    if (other.GetType() == ColliderType::Capsule) {
        auto& o = static_cast<const CapsuleCollider&>(other);
        return IsCollision(obb,o.capWorld_);
    }

    // OBB
    if (other.GetType() == ColliderType::OBB) {
        auto& o = static_cast<const OBBCollider&>(other);
        return IsCollision(obb,o.obb);
    }

    return false;
}

bool OBBCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
    if (!other.enabled) return false;

    return false;
}
#pragma endregion



