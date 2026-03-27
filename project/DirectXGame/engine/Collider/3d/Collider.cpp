#include "Collider.h"
#include"DirectXGame/engine/Camera/Camera.h"

#pragma region 球

void Engine::SphereCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
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

bool Engine::SphereCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;
	ColliderShapeType otherType = other.GetType();

	// 球
	if (otherType == ColliderShapeType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(Sphere{ centerWorld ,radius }, Sphere{ o.centerWorld, o.radius });
	}

	// AABB
	else if (otherType == ColliderShapeType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return Collision::Detection::Check(AABB(o.minWorld, o.maxWorld), Sphere{ centerWorld ,radius });
	}

	// カプセル
	else if (otherType == ColliderShapeType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);
		return Collision::Detection::Check(Sphere{ centerWorld ,radius }, o.capWorld_);
	}

	// OBB
	else if (otherType == ColliderShapeType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);
		return Collision::Detection::Check(o.obb, Sphere{ centerWorld ,radius });
	}

	// Ray
	else if (otherType == ColliderShapeType::Ray) {
		auto& o = static_cast<const RayCollider&>(other);
		return Collision::Detection::Check(o.ray_, Sphere{ centerWorld ,radius });
	}

	// 三角面
	else if (otherType == ColliderShapeType::Triangle) {
		auto& o = static_cast<const TriangleCollider&>(other);
		bool is = Collision::Detection::Check(o.GetWorldTriangle(), Sphere{ centerWorld ,radius });
		return is;
	}


	// AABBとの衝突など他の型は別で判定
	return false;
}

bool Engine::SphereCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const
{
	// 球
	if (other.GetType() == ColliderShapeType::Sphere) {
		const SphereCollider& o = static_cast<const SphereCollider&>(other);
		return Collision::Response::ReflectVelocity(Sphere{ centerWorld ,radius }, Sphere{ o.centerWorld, o.radius }, outPushVec);
	}

	// AABB
	else if (other.GetType() == ColliderShapeType::AABB) {
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
	else if (other.GetType() == ColliderShapeType::Capsule) {
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
	else if (other.GetType() == ColliderShapeType::OBB) {
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

	else if (other.GetType() == ColliderShapeType::Triangle) {
		const TriangleCollider& triangleColl = static_cast<const TriangleCollider&>(other);

		const Triangle triangle = triangleColl.GetWorldTriangle();
		const Vector3 sphereCenter = centerWorld;
		const float sphereRadius = radius;
		const Vector3 a = triangle.vertices[0];
		const Vector3 b = triangle.vertices[1];
		const Vector3 c = triangle.vertices[2];
		const float epsilon = 1e-6f;
		// 最近接点
		const Vector3 closest = ClosestPoint::SphereTriangle(Sphere{ sphereCenter,sphereRadius }, triangle);
		Vector3 diff = sphereCenter - closest;
		const float distSq = diff.Dot(diff);
		const float radiusSq = sphereRadius * sphereRadius;
		if (distSq > radiusSq) {
			return false;
		}

		Vector3 normal = (b - a).Cross(c - a);
		const float normalLenSq = normal.Dot(normal);
		if (normalLenSq <= epsilon) {
			return false;
		}
		normal /= std::sqrt(normalLenSq);

		// 法線方向押し戻し
		if (triangleColl.isNormal) {
			float signedDist = (sphereCenter - a).Dot(normal);

			if (signedDist < 0.0f) {
				normal = -normal;
				signedDist = -signedDist;
			}

			const float penetration = sphereRadius - signedDist;
			if (penetration <= 0.0f) {
				return false;
			}

			outPushVec = normal * penetration;
			return true;
		}

		// 最近点方向押し戻し
		if (distSq > epsilon) {
			const float dist = std::sqrt(distSq);
			const float penetration = sphereRadius - dist;
			outPushVec = (diff / dist) * penetration;
			return true;
		}

		// diff が潰れるケースだけ法線フォールバック
		float signedDist = (sphereCenter - a).Dot(normal);
		if (signedDist < 0.0f) {
			normal = -normal;
			signedDist = -signedDist;
		}

		const float penetration = sphereRadius - signedDist;
		if (penetration <= 0.0f) {
			return false;
		}

		outPushVec = normal * penetration;
		return true;
	}

	return false;
}
#pragma endregion 

#pragma region AABB
void Engine::AABBCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
{
	centerWorld = worldTransform.worldMat_.GetWorldPosition();
	minWorld = aabb.min + centerWorld;
	maxWorld = aabb.max + centerWorld;

#ifdef _DEBUG
	if (lineCommon) {
		if (isDebugLine) {
			if (enabled) {
				// AABBの最小・最大座標を使ってラインを描画
				lineCommon->GetDebugLineMeshData().AddLineAABB(aabb, centerWorld, { 1,1,1,1 });
			}
			else {
				// 無効な場合は透明にする
				lineCommon->GetDebugLineMeshData().AddLineAABB(aabb, centerWorld, { 0.5f,0.5f,0.5f,1.0f });
			}
		}
	}
#endif // _DEBUG

}

bool Engine::AABBCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	if (other.GetType() == ColliderShapeType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(AABB(minWorld, maxWorld), Sphere{ o.centerWorld ,o.radius });
	}

	// AABB
	else if (other.GetType() == ColliderShapeType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return (minWorld.x <= o.maxWorld.x && maxWorld.x >= o.minWorld.x) &&
			(minWorld.y <= o.maxWorld.y && maxWorld.y >= o.minWorld.y) &&
			(minWorld.z <= o.maxWorld.z && maxWorld.z >= o.minWorld.z);
	}

	// カプセル
	else if (other.GetType() == ColliderShapeType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);

		return Collision::Detection::Check(o.capWorld_, AABB(minWorld, maxWorld));
	}

	// OBB
	else  if (other.GetType() == ColliderShapeType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);

		return Collision::Detection::Check(o.obb, AABB(minWorld, maxWorld));
	}

	// OBB
	else if (other.GetType() == ColliderShapeType::Ray) {
		auto& o = static_cast<const RayCollider&>(other);
		return Collision::Detection::Check(o.ray_, AABB(minWorld, maxWorld));
	}

	return false;
}

bool Engine::AABBCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;

	if (other.GetType() == ColliderShapeType::Sphere) {
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
	else if (other.GetType() == ColliderShapeType::AABB) {
		const AABBCollider& o = static_cast<const AABBCollider&>(other);

		// 交差してなければスキップ
		if (!aabb.intersects(o.aabb)) return false;

		// 各軸の重なり量を求める
		float dx1 = aabb.max.x - o.aabb.min.x;
		float dx2 = o.aabb.max.x - aabb.min.x;
		float dx = (dx1 < dx2) ? dx1 : -dx2;

		float dy1 = aabb.max.y - o.aabb.min.y;
		float dy2 = o.aabb.max.y - aabb.min.y;
		float dy = (dy1 < dy2) ? dy1 : -dy2;

		float dz1 = aabb.max.z - o.aabb.min.z;
		float dz2 = o.aabb.max.z - aabb.min.z;
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
	else if (other.GetType() == ColliderShapeType::Capsule) {
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
	else if (other.GetType() == ColliderShapeType::OBB) {
	}

	return false;
}
#pragma endregion

#pragma region Capsule

void Engine::CapsuleCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
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

bool Engine::CapsuleCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;


	// 球
	if (other.GetType() == ColliderShapeType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(Sphere{ {o.centerWorld} ,{o.radius} }, capWorld_);
	}

	// AABB
	else if (other.GetType() == ColliderShapeType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return Collision::Detection::Check(capWorld_, AABB{ o.minWorld,o.maxWorld });
	}

	// カプセル
	else if (other.GetType() == ColliderShapeType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);
		return Collision::Detection::Check(capWorld_, o.capWorld_);
	}

	// OBB
	else if (other.GetType() == ColliderShapeType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);
		return Collision::Detection::Check(o.obb, capWorld_);
	}


	return false;
}

bool Engine::CapsuleCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;

	if (other.GetType() == ColliderShapeType::Sphere) {
	}
	if (other.GetType() == ColliderShapeType::AABB) {
	}
	if (other.GetType() == ColliderShapeType::Capsule) {
	}
	if (other.GetType() == ColliderShapeType::OBB) {
	}

	return false;
}
#pragma endregion

#pragma region OBB

void Engine::OBBCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
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

bool Engine::OBBCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	if (other.GetType() == ColliderShapeType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(obb, Sphere{ o.centerWorld,o.radius });
	}

	// AABB
	else if (other.GetType() == ColliderShapeType::AABB) {
		auto& o = static_cast<const AABBCollider&>(other);
		return Collision::Detection::Check(obb, AABB(o.minWorld, o.maxWorld));
	}

	// カプセル
	else if (other.GetType() == ColliderShapeType::Capsule) {
		auto& o = static_cast<const CapsuleCollider&>(other);
		return Collision::Detection::Check(obb, o.capWorld_);
	}

	// OBB
	else if (other.GetType() == ColliderShapeType::OBB) {
		auto& o = static_cast<const OBBCollider&>(other);
		return Collision::Detection::Check2(obb, o.obb);
	}

	return false;
}

bool Engine::OBBCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;

	if (other.GetType() == ColliderShapeType::Sphere) {
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
	if (other.GetType() == ColliderShapeType::AABB) {
		const AABBCollider& aabb = static_cast<const AABBCollider&>(other);
		Vector3 aabbCenter = (aabb.aabb.min + aabb.aabb.max) * 0.5f;
		Vector3 halfExtents = (aabb.aabb.max - aabb.aabb.min) * 0.5f;

		// AABBを仮想球として扱う（近似）
		SphereCollider tempSphere;
		tempSphere.centerWorld = aabbCenter;
		tempSphere.radius = halfExtents.Length() * 0.5f;

		return this->ResolveCollision(tempSphere, outPushVec); // 再帰的に使う
	}
	if (other.GetType() == ColliderShapeType::Capsule) {
		const CapsuleCollider& cap = static_cast<const CapsuleCollider&>(other);
		Vector3 closest = ClosestPoint::PointSegment(cap.capsule.segment, obb.center); // OBB中心から最接近点
		SphereCollider tempSphere;
		tempSphere.centerWorld = closest;
		tempSphere.radius = cap.capsule.radius;

		return this->ResolveCollision(tempSphere, outPushVec); // 再利用
	}
	if (other.GetType() == ColliderShapeType::OBB) {
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
void Engine::RayCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon)
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

bool Engine::RayCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	else if (other.GetType() == ColliderShapeType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(ray_, Sphere{ o.centerWorld,o.radius });
	}

	// AABB
	else if (other.GetType() == ColliderShapeType::AABB) {
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

bool Engine::RayCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const {
	if (!other.enabled) return false;
	return false;
}
#pragma endregion

#pragma region Triangle

void Engine::TriangleCollider::Update(const WorldTransform& worldTransform, LineCommon* lineCommon) {
	centerWorld = worldTransform.worldMat_.GetWorldPosition();
#ifdef _DEBUG
	if (lineCommon) {
		if (isDebugLine) {
			if (enabled) {
				// カプセルの線分と半径を使ってラインを描画
				lineCommon->GetDebugLineMeshData().AddLineTriangle({triangle01,triangle02 ,triangle03}, worldTransform);
			}
			else {
				// 無効な場合は透明にする
				
			}
		}
	}
#endif // _DEBUG
}

bool Engine::TriangleCollider::CheckHit(const Collider& other) const
{
	if (!other.enabled) return false;

	// 球
	else if (other.GetType() == ColliderShapeType::Sphere) {
		auto& o = static_cast<const SphereCollider&>(other);
		return Collision::Detection::Check(GetWorldTriangle(), Sphere{o.centerWorld,o.radius});
	}

	return false;
}

bool Engine::TriangleCollider::ResolveCollision(const Collider& other, Vector3& outPushVec) const
{

	if (other.GetType() == ColliderShapeType::Sphere) {
		const SphereCollider& sphere = static_cast<const SphereCollider&>(other);

		const Triangle triangle = GetWorldTriangle();
		const Vector3 sphereCenter = sphere.centerWorld;
		const float sphereRadius = sphere.radius;

		const Vector3 a = triangle.vertices[0];
		const Vector3 b = triangle.vertices[1];
		const Vector3 c = triangle.vertices[2];

		// 線分上最近点
		auto ClosestPointOnSegment = [](const Vector3& p, const Vector3& s0, const Vector3& s1) -> Vector3
			{
				const Vector3 seg = s1 - s0;
				const float segLenSq = seg.Dot(seg);

				if (segLenSq <= 1e-6f) {
					return s0;
				}

				float t = (p - s0).Dot(seg) / segLenSq;
				t = std::clamp(t, 0.0f, 1.0f);
				return s0 + seg * t;
			};

		// 三角形上最近点
		auto ClosestPointOnTriangle = [&](const Vector3& p) -> Vector3
			{
				const Vector3 ab = b - a;
				const Vector3 ac = c - a;
				const Vector3 ap = p - a;

				const float d1 = ab.Dot(ap);
				const float d2 = ac.Dot(ap);
				if (d1 <= 0.0f && d2 <= 0.0f) {
					return a; // 頂点 A 領域
				}

				const Vector3 bp = p - b;
				const float d3 = ab.Dot(bp);
				const float d4 = ac.Dot(bp);
				if (d3 >= 0.0f && d4 <= d3) {
					return b; // 頂点 B 領域
				}

				const float vc = d1 * d4 - d3 * d2;
				if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
					const float v = d1 / (d1 - d3);
					return a + ab * v; // 辺 AB
				}

				const Vector3 cp = p - c;
				const float d5 = ab.Dot(cp);
				const float d6 = ac.Dot(cp);
				if (d6 >= 0.0f && d5 <= d6) {
					return c; // 頂点 C 領域
				}

				const float vb = d5 * d2 - d1 * d6;
				if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
					const float w = d2 / (d2 - d6);
					return a + ac * w; // 辺 AC
				}

				const float va = d3 * d6 - d5 * d4;
				if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
					const Vector3 bc = c - b;
					const float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
					return b + bc * w; // 辺 BC
				}

				// 面内部
				const float denom = 1.0f / (va + vb + vc);
				const float v = vb * denom;
				const float w = vc * denom;
				return a + ab * v + ac * w;
			};

		const Vector3 closest = ClosestPointOnTriangle(sphereCenter);
		Vector3 diff = sphereCenter - closest;

		const float distSq = diff.Dot(diff);
		const float radiusSq = sphereRadius * sphereRadius;

		if (distSq > radiusSq) {
			return false;
		}

		// 侵入している
		const float epsilon = 1e-6f;

		if (distSq > epsilon) {
			const float dist = std::sqrt(distSq);
			const float penetration = sphereRadius - dist;
			outPushVec = diff / dist * penetration;
			return true;
		}

		// 球中心が最近点とほぼ一致しているケース
		// = 中心がちょうど面上 / 辺上 / 頂点上に近い
		// この場合は三角形法線方向へ押し出す
		Vector3 normal = (b - a).Cross(c - a);
		const float normalLenSq = normal.Dot(normal);

		if (normalLenSq <= epsilon) {
			// 退化三角形
			return false;
		}

		normal /= std::sqrt(normalLenSq);

		// 球中心が法線のどちら側にいるかで押し出し方向を決める
		const float signedDist = (sphereCenter - a).Dot(normal);
		if (signedDist < 0.0f) {
			normal = -normal;
		}

		outPushVec = normal * sphereRadius;
		return true;
	}

	return false;
}

#pragma endregion // 三角面
