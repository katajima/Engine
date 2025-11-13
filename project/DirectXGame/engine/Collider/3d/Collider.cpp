#include "Collider.h"
#include"DirectXGame/engine/Camera/Camera.h"

#pragma region 球

void SphereCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
	centerWorld = worldTransform.worldMat_.GetWorldPosition();

#ifdef _DEBUG
	if (lineCommon) {
		if (isDebugLine) {
			if (enabled) {
				// 球の中心位置と半径を使ってラインを描画
				lineCommon->GetDebugLineMeshData().AddLineSphere({ centerWorld ,radius }, { 1,1,1,1 }, 8, 8);
			}
			else {
				// 無効な場合は透明にする
				lineCommon->GetDebugLineMeshData().AddLineSphere({ centerWorld ,radius }, { 0.5f,0.5f,0.5f,1.0f }, 8, 8);
			}
		}
	}
#endif // _DEBUG
}

bool SphereCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	if (other.GetType() == ColliderType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(Sphere{ centerWorld ,radius }, Sphere{ o.centerWorld, o.radius });
	}

	// AABB
	else if (other.GetType() == ColliderType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return Collision::Detection::Check(AABB(o.minWorld, o.maxWorld), Sphere{ centerWorld ,radius });
	}

	// カプセル
	else if (other.GetType() == ColliderType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);
		return Collision::Detection::Check(Sphere{ centerWorld ,radius }, o.capWorld_);
	}

	// OBB
	else if (other.GetType() == ColliderType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);
		return Collision::Detection::Check(o.obb, Sphere{ centerWorld ,radius });
	}

	// Ray
	else if (other.GetType() == ColliderType::Ray) {
		auto& o = static_cast<const RayCollider&>(other);
		return Collision::Detection::Check(o.ray_, Sphere{ centerWorld ,radius });
	}



	// AABBとの衝突など他の型は別で判定
	return false;
}

bool SphereCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const
{
	// 球
	if (other.GetType() == ColliderType::Sphere) {
		const SphereCollider& o = static_cast<const SphereCollider&>(other);
		return Collision::Response::ReflectVelocity(Sphere{ centerWorld ,radius }, Sphere{ o.centerWorld, o.radius },outPushVec);
	}

	// AABB
	else if (other.GetType() == ColliderType::AABB) {
		const AABBCollider& o = static_cast<const AABBCollider&>(other);
		Vector3 closest = {
			std::clamp(centerWorld.x, o.minWorld.x, o.maxWorld.x),
			std::clamp(centerWorld.y, o.minWorld.y, o.maxWorld.y),
			std::clamp(centerWorld.z, o.minWorld.z, o.maxWorld.z)
		};

		Vector3 diff = centerWorld - closest;
		float distSq = diff.LengthSq();

		if (distSq < radius * radius && distSq > 0.00001f) {
			float dist = std::sqrt(distSq);
			float pushDepth = radius - dist;
			Vector3 pushDir = diff / dist;
			outPushVec = pushDir * pushDepth;
			return true;
		}
	}

	// Capsule
	else if (other.GetType() == ColliderType::Capsule) {
		const CapsuleCollider& o = static_cast<const CapsuleCollider&>(other);

		// 球の中心とカプセルの線分の最近接点を求める
		Vector3 closest = ClosestPoint::PointSegment(o.capsule.segment, centerWorld);
		Vector3 diff = centerWorld - closest;
		float distSq = diff.LengthSq();
		float radiusSum = radius + o.capsule.radius;

		if (distSq < radiusSum * radiusSum && distSq > 0.00001f) {
			float dist = std::sqrt(distSq);
			float pushDepth = radiusSum - dist;
			Vector3 pushDir = diff / dist;
			outPushVec = pushDir * pushDepth;
			return true;
		}
	}

	// OBB
	else if (other.GetType() == ColliderType::OBB) {
		const OBBCollider& o = static_cast<const OBBCollider&>(other);

		// 球の中心をOBBのローカル空間に変換
		Matrix4x4 invMat = Inverse(OBB::MakeOBBMatrix(o.obb)); // OBBのワールド行列の逆行列
		Vector3 localCenter = invMat.Transform(centerWorld);

		// ローカルAABBとの最近接点
		Vector3 localClosest = {
			std::clamp(localCenter.x, -o.obb.size.x * 1.0f, o.obb.size.x * 1.0f),
			std::clamp(localCenter.y, -o.obb.size.y * 1.0f, o.obb.size.y * 1.0f),
			std::clamp(localCenter.z, -o.obb.size.z * 1.0f, o.obb.size.z * 1.0f)
		};

		// ワールド空間に戻して差分・判定
		Vector3 worldClosest = OBB::MakeOBBMatrix(o.obb).Transform(localClosest);
		Vector3 diff = centerWorld - worldClosest;
		float distSq = diff.LengthSq();

		if (distSq < radius * radius && distSq > 0.00001f) {
			float dist = std::sqrt(distSq);
			float pushDepth = radius - dist;
			Vector3 pushDir = diff / dist;
			outPushVec = pushDir * pushDepth;
			return true;
		}
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

	//Vector3 size = aabb.min_;//; +aabb.max_;

#ifdef _DEBUG
	if (lineCommon) {
		if (isDebugLine) {
			if (enabled) {
				// AABBの最小・最大座標を使ってラインを描画
				lineCommon->GetDebugLineMeshData().AddLineAABB(aabb, centerWorld, { 1,1,1,1 });
				//OBB obb = { {centerWorld},{},{size} };
				//lineCommon->AddLineOBB(obb, { 1,1,1,1 });
			}
			else {
				// 無効な場合は透明にする
				lineCommon->GetDebugLineMeshData().AddLineAABB(aabb, centerWorld, { 0.5f,0.5f,0.5f,1.0f });
			}
		}
	}
#endif // _DEBUG

}

bool AABBCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	if (other.GetType() == ColliderType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(AABB(minWorld, maxWorld), Sphere{ o.centerWorld ,o.radius });
	}

	// AABB
	else if (other.GetType() == ColliderType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return (minWorld.x <= o.maxWorld.x && maxWorld.x >= o.minWorld.x) &&
			(minWorld.y <= o.maxWorld.y && maxWorld.y >= o.minWorld.y) &&
			(minWorld.z <= o.maxWorld.z && maxWorld.z >= o.minWorld.z);
	}

	// カプセル
	else if (other.GetType() == ColliderType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);

		return Collision::Detection::Check(o.capWorld_, AABB(minWorld, maxWorld));
	}

	// OBB
	else  if (other.GetType() == ColliderType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);

		return Collision::Detection::Check(o.obb, AABB(minWorld, maxWorld));
	}

	// OBB
	else if (other.GetType() == ColliderType::Ray) {
		auto& o = static_cast<const RayCollider&>(other);
		return Collision::Detection::Check(o.ray_, AABB(minWorld, maxWorld));
	}

	return false;
}

bool AABBCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;

	if (other.GetType() == ColliderType::Sphere) {
		const SphereCollider& sphere = static_cast<const SphereCollider&>(other);

		// AABBの最近接点を求める
		Vector3 closestPoint = Vector3::Clamp(sphere.centerWorld, minWorld, maxWorld);

		Vector3 diff = sphere.centerWorld - closestPoint;
		float distSq = diff.LengthSq();
		float radius = sphere.radius;

		if (distSq < radius * radius) {
			float dist = std::sqrt(distSq);
			if (dist > 0.0001f) {
				Vector3 pushDir = diff / dist;
				float pushDepth = radius - dist;
				outPushVec = pushDir * pushDepth;
				return true;
			}
		}
	}
	else if (other.GetType() == ColliderType::AABB) {
		const AABBCollider& o = static_cast<const AABBCollider&>(other);

		// 交差してなければスキップ
		if (!aabb.intersects(o.aabb)) return false;

		// 各軸の重なり量を求める
		float dx1 = aabb.max_.x - o.aabb.min_.x;
		float dx2 = o.aabb.max_.x - aabb.min_.x;
		float dx = (dx1 < dx2) ? dx1 : -dx2;

		float dy1 = aabb.max_.y - o.aabb.min_.y;
		float dy2 = o.aabb.max_.y - aabb.min_.y;
		float dy = (dy1 < dy2) ? dy1 : -dy2;

		float dz1 = aabb.max_.z - o.aabb.min_.z;
		float dz2 = o.aabb.max_.z - aabb.min_.z;
		float dz = (dz1 < dz2) ? dz1 : -dz2;

		// 最も小さい押し戻し方向を使う
		if (std::abs(dx) < std::abs(dy) && std::abs(dx) < std::abs(dz)) {
			outPushVec = Vector3(dx, 0, 0);
		}
		else if (std::abs(dy) < std::abs(dz)) {
			outPushVec = Vector3(0, dy, 0);
		}
		else {
			outPushVec = Vector3(0, 0, dz);
		}
		return true;
	}
	else if (other.GetType() == ColliderType::Capsule) {
		const CapsuleCollider& capsule = static_cast<const CapsuleCollider&>(other);
		const Segment& seg = capsule.capsule.segment;

		// 線分の最近接点をAABBに対して求める
		Vector3 closest = ClosestPoint::SegmentAABB(seg.origin, seg.end, aabb);

		Vector3 diff = capsule.capsule.segment.ClosestPoint(closest) - closest;
		float distSq = diff.LengthSq();
		float r = capsule.capsule.radius;

		if (distSq < r * r) {
			float dist = std::sqrt(distSq);
			if (dist > 0.0001f) {
				Vector3 pushDir = diff / dist;
				float pushDepth = r - dist;
				outPushVec = pushDir * pushDepth;
				return true;
			}
		}
	}
	else if (other.GetType() == ColliderType::OBB) {
	}

	return false;
}
#pragma endregion

#pragma region Capsule

void CapsuleCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
	centerWorld = worldTransform.worldMat_.GetWorldPosition();

	capWorld_ = { Vector3{capsule.segment.origin + centerWorld},Vector3{capsule.segment.end + centerWorld},{capsule.radius} };

#ifdef _DEBUG
	if (lineCommon) {
		if (isDebugLine) {
			if (enabled) {
				// カプセルの線分と半径を使ってラインを描画
				lineCommon->GetDebugLineMeshData().AddLineCapsule(capWorld_, { 1,1,1,1 });
			}
			else {
				// 無効な場合は透明にする
				lineCommon->GetDebugLineMeshData().AddLineCapsule(capWorld_, { 0.5f,0.5f,0.5f,1.0f });
			}
		}
	}
#endif // _DEBUG

}

bool CapsuleCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;


	// 球
	if (other.GetType() == ColliderType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(Sphere{ {o.centerWorld} ,{o.radius} }, capWorld_);
	}

	// AABB
	else if (other.GetType() == ColliderType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return Collision::Detection::Check(capWorld_, AABB{ o.minWorld,o.maxWorld });
	}

	// カプセル
	else if (other.GetType() == ColliderType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);
		return Collision::Detection::Check(capWorld_, o.capWorld_);
	}

	// OBB
	else if (other.GetType() == ColliderType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);
		return Collision::Detection::Check(o.obb, capWorld_);
	}


	return false;
}

bool CapsuleCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;

	if (other.GetType() == ColliderType::Sphere) {
	}
	if (other.GetType() == ColliderType::AABB) {
	}
	if (other.GetType() == ColliderType::Capsule) {
	}
	if (other.GetType() == ColliderType::OBB) {
	}

	return false;
}
#pragma endregion

#pragma region OBB

void OBBCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
	centerWorld = worldTransform.worldMat_.GetWorldPosition();

	obb.center = centerWorld;
	obb.orientations[0] = worldTransform.worldMat_.AxisRow(0).Normalize();
	obb.orientations[1] = worldTransform.worldMat_.AxisRow(1).Normalize();
	obb.orientations[2] = worldTransform.worldMat_.AxisRow(2).Normalize();

#ifdef _DEBUG
	if (lineCommon) {
		if (isDebugLine) {
			lineCommon->GetDebugLineMeshData().AddLine(obb.center, obb.center + obb.orientations[0], { 1,0,0,1 }); // X軸: 赤
			lineCommon->GetDebugLineMeshData().AddLine(obb.center, obb.center + obb.orientations[1], { 0,1,0,1 }); // Y軸: 緑
			lineCommon->GetDebugLineMeshData().AddLine(obb.center, obb.center + obb.orientations[2], { 0,0,1,1 }); // Z軸: 青
			if (obb.size.x > 0 && obb.size.y > 0 && obb.size.z > 0) {
				// OBBのサイズを使ってラインを描画
				lineCommon->GetDebugLineMeshData().AddLineOBB(obb, { 1,1,1,1 });
			}
			else {
				// 無効な場合は透明にする
				lineCommon->GetDebugLineMeshData().AddLineOBB(obb, { 0.0f,0.0f,0.0f,1.0f });
			}
		}
	}
#endif // _DEBUG

}

bool OBBCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	if (other.GetType() == ColliderType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(obb, Sphere{ o.centerWorld,o.radius });
	}

	// AABB
	else if (other.GetType() == ColliderType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return Collision::Detection::Check(obb, AABB(o.minWorld, o.maxWorld));
	}

	// カプセル
	else if (other.GetType() == ColliderType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);
		return Collision::Detection::Check(obb, o.capWorld_);
	}

	// OBB
	else if (other.GetType() == ColliderType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);
		return Collision::Detection::Check2(obb, o.obb);
	}

	return false;
}

bool OBBCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;

	if (other.GetType() == ColliderType::Sphere) {
		const SphereCollider& sphere = static_cast<const SphereCollider&>(other);

		Matrix4x4 obbWorld = OBB::MakeOBBMatrix(obb);
		Matrix4x4 invWorld = Inverse(obbWorld);

		// 球の中心をOBBのローカル空間へ変換
		Vector3 localCenter = invWorld.Transform(sphere.centerWorld);

		// OBBのローカルAABBで最近接点を求める
		Vector3 localClosest = {
			std::clamp(localCenter.x, -obb.size.x * 1.0f, obb.size.x * 1.0f),
			std::clamp(localCenter.y, -obb.size.y * 1.0f, obb.size.y * 1.0f),
			std::clamp(localCenter.z, -obb.size.z * 1.0f, obb.size.z * 1.0f),
		};

		// ワールド空間に戻して判定
		Vector3 worldClosest = obbWorld.Transform(localClosest);
		Vector3 diff = sphere.centerWorld - worldClosest;
		float distSq = diff.LengthSq();

		if (distSq < sphere.radius * sphere.radius && distSq > 0.00001f) {
			float dist = std::sqrt(distSq);
			Vector3 pushDir = diff / dist;
			float pushDepth = sphere.radius - dist;
			outPushVec = -pushDir * pushDepth;
			return true;
		}
	}
	if (other.GetType() == ColliderType::AABB) {
		const AABBCollider& aabb = static_cast<const AABBCollider&>(other);
		Vector3 aabbCenter = (aabb.aabb.min_ + aabb.aabb.max_) * 0.5f;
		Vector3 halfExtents = (aabb.aabb.max_ - aabb.aabb.min_) * 0.5f;

		// AABBを仮想球として扱う（近似）
		SphereCollider tempSphere;
		tempSphere.centerWorld = aabbCenter;
		tempSphere.radius = halfExtents.Length() * 0.5f;

		return this->ResolveCollision(tempSphere, outPushVec); // 再帰的に使う
	}
	if (other.GetType() == ColliderType::Capsule) {
		const CapsuleCollider& cap = static_cast<const CapsuleCollider&>(other);
		Vector3 closest = ClosestPoint::PointSegment(cap.capsule.segment, obb.center); // OBB中心から最接近点
		SphereCollider tempSphere;
		tempSphere.centerWorld = closest;
		tempSphere.radius = cap.capsule.radius;

		return this->ResolveCollision(tempSphere, outPushVec); // 再利用
	}
	if (other.GetType() == ColliderType::OBB) {
		//const OBBCollider& otherObb = static_cast<const OBBCollider&>(other);
		//SATResult sat = CheckOBBCollisionSAT(this->obb, otherObb.obb);

		//if (!sat.hit) return false;

		//// 安全チェック
		//if (sat.minOverlap <= 0.0f || !std::isfinite(sat.minOverlap)) return false;
		//if (sat.pushDir.LengthSq() < 0.0001f || !std::isfinite(sat.pushDir.x)) return false;

		//// 最大押し戻し制限付き
		//outPushVec = sat.pushDir * std::clamp(sat.minOverlap, 0.0f, 1.0f);
		//return true;
	}


	return false;
}
#pragma endregion

#pragma region Ray
void RayCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
	centerWorld = worldTransform.worldMat_.GetWorldPosition();

	ray_.origin = centerWorld;

#ifdef _DEBUG
	if (lineCommon) {
		if (isDebugLine) {
			lineCommon->GetDebugLineMeshData().AddLine(ray_.origin, ray_.origin + ray_.diff, { 1,1,1,1 });
		}
	}
#endif // _DEBUG

}

bool RayCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	else if (other.GetType() == ColliderType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(ray_, Sphere{ o.centerWorld,o.radius });
	}

	// AABB
	else if (other.GetType() == ColliderType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return Collision::Detection::Check(ray_, AABB(o.minWorld, o.maxWorld));
	}

	//// カプセル
	//if (other.GetType() == ColliderType::Capsule) {
	//	auto& o = static_cast<const CapsuleCollider&>(other);
	//	return IsCollision(obb, o.capWorld_);
	//}

	//// OBB
	//if (other.GetType() == ColliderType::OBB) {
	//	auto& o = static_cast<const OBBCollider&>(other);
	//	return IsCollision2(obb, o.obb);
	//}

	return false;
}

bool RayCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;
	return false;
}
#pragma endregion

