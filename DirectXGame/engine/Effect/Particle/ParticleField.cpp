#include "ParticleField.h"

#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Collider/3d/ColliderFanction3D.h"

#include "imgui.h"

void Field::FieldEffect::Init(const std::string& name, ShapeType shapeType, EffectType type, LineCommon* lineCommon)
{
	lineCommon_ = lineCommon;
	name_ = name;
	effectType_ = type;
	shapeType_ = shapeType;
	transform_.Initialize();




	switch (effectType_)
	{
	case Field::EffectType::kGravity:
		nameType = "Gravity";
		break;
	case Field::EffectType::kAcceleration:
		nameType = "Acceleration";
		break;
	case Field::EffectType::kDestruction:
		nameType = "Destruction";
		break;
	case Field::EffectType::kColor:
		nameType = "Color";
		break;
	default:
		break;
	}

	switch (shapeType_)
	{
	case Field::ShapeType::kAABB:
		name_ = name_ + "AABB";

		renge_.min_ = -Vector3{ 1.0f,1.0f,1.0f };
		renge_.max_ = Vector3{ 1.0f,1.0f,1.0f };


		break;
	case Field::ShapeType::kSphere:
		name_ = name_ + "Sphere";
		rad = 5.0f;
		break;
	case Field::ShapeType::kCapsule:
		name_ = name_ + "Capsule";

		rad = 5.0f;
		break;
	case Field::ShapeType::kCapsuleSpline:
		name_ = name_ + "CapsuleSpline";

		controlPoints.push_back({ 0,0,0 });
		controlPoints.push_back({ 10,0,0 });
		controlPoints.push_back({ 20,0,0 });
		controlPoints.push_back({ 30,0,0 });


		rad = 5.0f;
		break;
	default:
		break;
	}

}

void Field::FieldEffect::Update()
{
	transform_.Update();
}

void Field::FieldEffect::DebugImgui()
{
	if (ImGui::TreeNode(name_.c_str())) {
		ImGui::Text(nameType.c_str());
		ImGui::DragFloat3("translation", &transform_.translate_.x, 0.1f);
		ImGui::Checkbox("isEffect", &isEffect);
		ImGui::Separator();
		switch (shapeType_)
		{
		case Field::ShapeType::kAABB:
			ImGui::DragFloat3("renge.max", &renge_.max_.x, 0.1f);
			ImGui::DragFloat3("renge.min", &renge_.min_.x, 0.1f);
			break;
		case Field::ShapeType::kSphere:
			ImGui::DragFloat("rad", &rad, 0.1f);
			break;
		case Field::ShapeType::kCapsule:
			ImGui::DragFloat("rad", &rad, 0.1f);
			break;
		case Field::ShapeType::kCapsuleSpline:
			ImGui::DragFloat("rad", &rad, 0.1f);
			if (ImGui::CollapsingHeader("Sprine")) {
				for (int i = 0; i < controlPoints.size(); i++) {
					std::string index = std::to_string(i);
					ImGui::DragFloat3(index.c_str(), &controlPoints[i].x, 0.1f);
				}
			}
			break;
		default:
			break;
		}
		ImGui::Separator();
		switch (effectType_)
		{
		case Field::EffectType::kGravity:
			ImGui::DragFloat("force", &force_, 0.1f);
			break;
		case Field::EffectType::kAcceleration:
			ImGui::DragFloat3("acceleration", &acceleration_.x, 0.1f);
			break;
		case Field::EffectType::kDestruction:
			break;
		case Field::EffectType::kColor:
			ImGui::ColorEdit4("color", &color_.x);
			break;
		default:
			break;
		}
		ImGui::TreePop();
	}
	// ライン
	lineCommon_->AddLineAABB(renge_, transform_.translate_, color_);
}

bool Field::FieldEffect::IsCollisionAABB(const Vector3& point)
{
	switch (shapeType_)
	{
	case Field::ShapeType::kAABB:
		return IsCollision(AABB{ renge_.min_ + transform_.translate_,renge_.max_ + transform_.translate_ }, point);
		break;
	case Field::ShapeType::kSphere:
		return IsCollision(Sphere{ transform_.translate_,rad }, point);
		break;
	case Field::ShapeType::kCapsule:
		return IsCollision(AABB{ renge_.min_ + transform_.translate_,renge_.max_ + transform_.translate_ }, point);
		break;
	case Field::ShapeType::kCapsuleSpline:
		return IsCollision(AABB{ renge_.min_ + transform_.translate_,renge_.max_ + transform_.translate_ }, point);
		break;
	default:
		break;
	}
	return false;
}

void Field::Effect(ParticleGroup& grop, std::list<Particle>::iterator& particleIterator, std::vector<Field::FieldEffect>& fieldEffect, float deltaTime)
{
	for (auto& acc : fieldEffect) {

		if (acc.GetIsEffect()) { // 影響を出すか
			if (acc.IsCollisionAABB(particleIterator->transform.translate)) {
				switch (acc.effectType_) {
				case Field::EffectType::kGravity:
					Vector3 def = particleIterator->transform.translate - acc.transform_.translate_;
					particleIterator->velocity += (def.Normalize() * acc.force_) * deltaTime;
					break;
				case Field::EffectType::kAcceleration:
					particleIterator->velocity += acc.acceleration_ * deltaTime;
					break;
				case Field::EffectType::kDestruction:
					particleIterator->isDestroy = true;
					break;
				case Field::EffectType::kColor:
					particleIterator->color = acc.color_;
					break;
				}
			}
		}
		acc.Update();
	}

}
