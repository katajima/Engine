#include "Collider2D.h"

void SphereCollider2D::Update(const WorldTransform2d& worldTransform)
{
}

bool SphereCollider2D::CheckHit(const Collider2D& other) const
{
	return false;
}

bool SphereCollider2D::ResolveCollision(const Collider2D& other, Vector2& outPushVec) const
{
	return false;
}


void AABBCollider2D::Update(const WorldTransform2d& worldTransform) {

	minWorld = worldTransform.translate_ + box.min_;
	maxWorld = worldTransform.translate_ + box.max_;


};
bool AABBCollider2D::CheckHit(const Collider2D& other) const {

	if (!other.enabled) return false;

	//// 球
	//if (other.GetType() == ColliderType::Sphere) {
	//	auto& o = static_cast<const SphereCollider&>(other);
	//	float distSq = (centerWorld - o.centerWorld).LengthSq();
	//	float radiusSum = radius + o.radius;
	//	return distSq <= radiusSum * radiusSum;
	//}



	// AABB
	if (other.GetType() == ColliderType::AABB) {
		auto& o = static_cast<const AABBCollider2D&>(other);
		return IsCollision(Box(o.minWorld, o.maxWorld), Box{ minWorld ,maxWorld });
	}
	return false;
};


bool AABBCollider2D::ResolveCollision(const Collider2D& other, Vector2& outPushVec) const {

    if (!other.enabled) return false;

    if (other.GetType() == ColliderType::AABB) {
        auto& o = static_cast<const AABBCollider2D&>(other);

        if (!IsCollision(Box(o.minWorld, o.maxWorld), Box{ minWorld, maxWorld })) {
            return false;
        }

        // 自分と相手の中心を計算
        Vector2 centerSelf = (minWorld + maxWorld) * 0.5f;
        Vector2 centerOther = (o.minWorld + o.maxWorld) * 0.5f;

        // 各軸での重なり量を計算
        float overlapX = (std::min)(maxWorld.x, o.maxWorld.x) - (std::max)(minWorld.x, o.minWorld.x);
        float overlapY = (std::min)(maxWorld.y, o.maxWorld.y) - (std::max)(minWorld.y, o.minWorld.y);

        // 小さい方の軸で押し戻し
        if (overlapX < overlapY) {
            // X方向に押し出し
            if (centerSelf.x < centerOther.x) {
                outPushVec = { -overlapX, 0.0f }; // 自分は左側 → 左に押し戻す
            }
            else {
                outPushVec = { overlapX, 0.0f };  // 自分は右側 → 右に押し戻す
            }
        }
        else {
            // Y方向に押し出し
            if (centerSelf.y < centerOther.y) {
                outPushVec = { 0.0f, -overlapY }; // 自分は下側 → 下に押し戻す
            }
            else {
                outPushVec = { 0.0f, overlapY };  // 自分は上側 → 上に押し戻す
            }
        }

        return true;
    }

    return false;
};