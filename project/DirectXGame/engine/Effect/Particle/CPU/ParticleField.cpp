#include "ParticleField.h"

#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Collider/3d/ColliderFanction3D.h"
#include "DirectXGame/engine/Math/Random.h"
#include "DirectXGame/engine/Utility/RangeUtility.h"
#include "imgui.h"

void Field::FieldEffect::Initialize(const std::string& name, ShapeType shapeType, EffectType type, LineCommon* lineCommon)
{
	lineCommon_ = lineCommon;
	name_ = name;
	effectType_ = type;
	shapeType_ = shapeType;
	transform_.Initialize();

	noise_.InitParameters();

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
	case Field::EffectType::kDeceleration:
		nameType = "Deceleration";
		break;
	case Field::EffectType::kNoise:
		nameType = "Noise";
		break;
	case Field::EffectType::kTornado:
		nameType = "Tornado";
		break;
	case Field::EffectType::kEvent:
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
		Vector3 worldPos = transform_.worldMat_.GetWorldPosition();
		ImGui::InputFloat3("worldPos", &worldPos.x);
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
			if (ImGui::CollapsingHeader("Spline")) {
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
		case Field::EffectType::kDeceleration:
			ImGui::SliderFloat("deceleration", &deceleration_, 0.0f, 1.0f);
			break;
		case Field::EffectType::kNoise:
			ImGui::DragFloat("noiseScale", &noiseScale_, 0.01f);
			ImGui::DragFloat3("rondomRenge.max", &rondomRenge.max.x, 0.1f);
			ImGui::DragFloat3("rondomRenge.min", &rondomRenge.min.x, 0.1f);
			break;
		case Field::EffectType::kTornado:
			ImGui::DragFloat("force", &force_, 0.1f);
			ImGui::DragFloat("lift", &lift_, 0.1f);
			break;

		case Field::EffectType::kEvent:
			ImGui::DragFloat("force", &force_, 0.1f);
			ImGui::DragFloat("lift", &lift_, 0.1f);
			break;
		default:
			break;
		}
		ImGui::TreePop();
	}

	switch (shapeType_)
	{
	case Field::ShapeType::kAABB:
		// ライン
		lineCommon_->AddLineAABB(renge_, transform_.worldMat_.GetWorldPosition(), color_);
		break;
	case Field::ShapeType::kSphere:
		// ライン
		lineCommon_->AddLineSphere(Sphere{ transform_.worldMat_.GetWorldPosition(),rad }, color_, 16, 16);
		break;
	case Field::ShapeType::kCapsule:
		break;
	case Field::ShapeType::kCapsuleSpline:
		break;
	default:
		break;
	}
}

bool Field::FieldEffect::IsCollisionAABB(const Vector3& point)
{
	Vector3 worldPos = transform_.worldMat_.GetWorldPosition();

	switch (shapeType_)
	{
	case Field::ShapeType::kAABB:
		return Collision::Detection::Check(AABB{ renge_.min_ + worldPos,renge_.max_ + worldPos }, point);
		break;
	case Field::ShapeType::kSphere:
		return Collision::Detection::Check(Sphere{ worldPos,rad }, point);
		break;
	case Field::ShapeType::kCapsule:
		return Collision::Detection::Check(AABB{ renge_.min_ + worldPos,renge_.max_ + worldPos }, point);
		break;
	case Field::ShapeType::kCapsuleSpline:
		return Collision::Detection::Check(AABB{ renge_.min_ + worldPos,renge_.max_ + worldPos }, point);
		break;
	default:
		break;
	}
	return false;
}

void Field::Effect(ParticleGroup& grop, std::list<Particle>::iterator& particleIterator, std::vector<Field::FieldEffect*> fieldEffect, float deltaTime)
{


	for (auto& acc : fieldEffect) {
		bool isEffect = false;
		for (auto& name : particleIterator->fieldEffect) {
			if (acc->GetName() == name) {
				isEffect = true;
			};

			if (name == "") {
				isEffect = true;
			}

			if (name == "All") {
				isEffect = true;
			}

			if (name == "false") {
				isEffect = false;
			}

		}

		if (!isEffect) continue; // 効果が適用されない場合はスキップ

		if (acc->GetIsEffect()) { // 影響を出すか
			if (acc->IsCollisionAABB(particleIterator->transform.translate)) {
				switch (acc->effectType_) {
				case Field::EffectType::kGravity:
					Vector3 def = particleIterator->transform.translate - acc->transform_.translate_;
					particleIterator->velocity += (def.Normalize() * acc->force_) * deltaTime;
					break;
				case Field::EffectType::kAcceleration:
					particleIterator->velocity += acc->acceleration_ * deltaTime;
					break;
				case Field::EffectType::kDestruction:
					particleIterator->isDestroy = true;
					break;
				case Field::EffectType::kColor:
					particleIterator->color = acc->color_;
					break;
				case Field::EffectType::kDeceleration:

					particleIterator->velocity.x *= acc->deceleration_;
					particleIterator->velocity.y *= acc->deceleration_;
					particleIterator->velocity.z *= acc->deceleration_;
					break;
				case Field::EffectType::kNoise:

					ConversionRange(acc->rondomRenge);

					particleIterator->velocity.x += Random::RandomFloat(acc->rondomRenge.min.x, acc->rondomRenge.max.x) * acc->noiseScale_;
					particleIterator->velocity.y += Random::RandomFloat(acc->rondomRenge.min.y, acc->rondomRenge.max.y) * acc->noiseScale_;
					particleIterator->velocity.z += Random::RandomFloat(acc->rondomRenge.min.z, acc->rondomRenge.max.z) * acc->noiseScale_;
					break;
				case Field::EffectType::kEvent:
					particleIterator->isEvent = true;
					break;
				case Field::EffectType::kTornado:
					Vector3 pos = particleIterator->transform.translate;
					Vector3 toCenter = acc->transform_.translate_ - pos;

					// Y軸方向成分を除いてXZ平面に投影
					Vector3 toCenterXZ = Vector3(toCenter.x, 0.0f, toCenter.z);
					float distXZ = toCenterXZ.Length();

					// 吸引力（中心に向かう）
					Vector3 radialForce = toCenterXZ.Normalize() * acc->force_;

					// 接線方向の回転力（左手系なのでZ→-X, X→Z で反時計回り）
					Vector3 tangentForce = Vector3(-toCenterXZ.z, 0.0f, toCenterXZ.x).Normalize() * acc->force_;

					// 上昇力（Y方向）
					Vector3 liftForce = Vector3(0.0f, acc->lift_, 0.0f);

					// 全て加算
					particleIterator->velocity += (radialForce + tangentForce + liftForce) * deltaTime;
					break;
				}
			}
		}
		acc->Update();
	}

}