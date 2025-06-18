#include "Collider.h"



void Collider::Initialize(Camera* camera) 
{
	capsule_.radius = radius_;
	capsule_.segment.origin = { 0,1.0f,0 };
	capsule_.segment.end = { 0,-1.0f,0 };
	color_ = { 1,1,1,1 };
}

void Collider::UpdateWorldTransform(LineCommon* lineCommon) {


	if (colliderTypeID_ == static_cast<uint32_t>(ColliderType::Sphere)) {
		lineCommon->AddLineSphere(Sphere{GetCenterPosition(), radius_ }, color_, 5, 5);
	}
	if (colliderTypeID_ == static_cast<uint32_t>(ColliderType::AABB)) {
		lineCommon->AddLineAABB(aabb_, GetCenterPosition(), color_);
	}
	if (colliderTypeID_ == static_cast<uint32_t>(ColliderType::OBB)) {
		lineCommon->AddLineOBB(obb_, color_);
	}
	if (colliderTypeID_ == static_cast<uint32_t>(ColliderType::Capsule)) {
		Capsule cap = capsule_;
		cap.segment.origin += GetCenterPosition();
		cap.segment.end += GetCenterPosition();
		lineCommon->AddLineCapsule(cap, color_);
	}

}

void Collider::Draw() {

}

void Collider::SetTypeID(uint32_t typeID) { typeID_ = typeID; };
