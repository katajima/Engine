#include "Primitive.h"

#include"PrimitiveCommon.h"

void BasePrimitive::Initialize(PrimitiveCommon* primitiveCommon, const std::string& tex, const Color color, const std::string& name, bool isLine)
{
	primitiveCommon_ = primitiveCommon;

	mesh = std::make_unique<ModelMesh>();
	MeshInitialize();

	mesh->Initialize(primitiveCommon_->GetDxCommon());

	mesh->material = std::make_unique<Material>();
	mesh->material->Initialize(primitiveCommon_->GetDxCommon());
	mesh->material->tex_.diffuseFilePath = tex;
	mesh->material->color = color;


	if (name == "") {
		name_ = "NoName";
	}
	else {
		name_ = name;
	}
}

void BasePrimitive::Update(float deltaTime)
{
#ifdef _DEBUG
	std::string str = name_ + "_translate";

	int i = (int)mesh->vertices.size();
	ImGui::InputInt("index2", &i);
	str = name_ + "material";
	ImGui::DragFloat3(str.c_str(), &mesh->material->transform.scale.x, 0.01f);
	str += "rotate";
	ImGui::DragFloat3(str.c_str(), &mesh->material->transform.rotate.x, 0.01f);

	ImGui::Checkbox("isScaleX", &aimetion_.isScaleX);
	ImGui::Checkbox("isScaleY", &aimetion_.isScaleY);
	ImGui::DragFloat2("speed", &aimetion_.speed.x, 0.01f);
	ImGui::DragFloat2("maxCount", &aimetion_.maxCount.x, 0.01f);

	ImGui::Checkbox("isRotateX", &aimetion_.isRotateX);
	ImGui::Checkbox("isRotateY", &aimetion_.isRotateY);
	ImGui::DragFloat2("rotateSpeed", &aimetion_.rotateSpeed.x, 0.01f);
	ImGui::DragFloat2("maxRotate", &aimetion_.maxRotate.x, 0.01f); 
#endif // _DEBUG

	if (aimetion_.isScaleX) {
		mesh->material->transform.scale.x += aimetion_.speed.x;
		if (mesh->material->transform.scale.x >= aimetion_.maxCount.x) {
			mesh->material->transform.scale.x = 0;
		}

	}
	if (aimetion_.isScaleY) {
		mesh->material->transform.scale.y += aimetion_.speed.y;
		if (mesh->material->transform.scale.y >= aimetion_.maxCount.y) {
			mesh->material->transform.scale.y = 0;
		}
	}

	if (aimetion_.isRotateX) {
		mesh->material->transform.rotate.x += aimetion_.rotateSpeed.x;
		if (mesh->material->transform.rotate.x >= aimetion_.maxRotate.x) {
			mesh->material->transform.rotate.x = 0;
		}

	}
	if (aimetion_.isRotateY) {
		mesh->material->transform.rotate.y += aimetion_.rotateSpeed.y;
		if (mesh->material->transform.rotate.y >= aimetion_.maxRotate.y) {
			mesh->material->transform.rotate.y = 0;
		}
	}

	mesh->material->GPUData();

	UniqeUpdate();
}

void BasePrimitive::Draw()
{
	if (mesh->vertices.size() != 0) {

		mesh->material->GetCommandListMaterial(0);

		mesh->material->GetCommandListTexture(2, 7, 8);



		mesh->GetCommandList();

		primitiveCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);
	}
};

void BasePrimitive::DrawSetting(PsoType type)
{
	switch (type)
	{
	case BasePrimitive::PsoType::kDefalt:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kDefalt);
		break;
	case BasePrimitive::PsoType::kRingClamp:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kRingClamp);
		break;
	case BasePrimitive::PsoType::kNoCull:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kNoCull);
		break;
	case BasePrimitive::PsoType::kNoCullRingClamp:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kNoCullRingClamp);
		break;
	case BasePrimitive::PsoType::kNoCullWireFrame:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kNoCullWireFrame);
		break;
	default:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kDefalt);
		break;
	}
}

void PlanePrimitive::UniqeUpdate()
{
	prePlane = plane;
}

void TrianglePrimitive::UniqeUpdate()
{
	if (triangle != preTriangle) {
		triangle.Create(mesh.get());
	}

	preTriangle = triangle;
}

void CirclePrimitive::UniqeUpdate()
{
	if (preCircle != circle) {
		circle.Create(mesh.get());
	}

	preCircle = circle;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Circle")) {
		ImGui::DragInt("segments", &circle.segments);
		ImGui::DragFloat("radius", &circle.radius, 0.1f);
	}
#endif // _DEBUG
}

void CubePrimitive::UniqeUpdate()
{
	if (preCube != cube) {
		cube.Create(mesh.get());
	}

	preCube = cube;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Cube")) {
		ImGui::DragFloat3("size", &cube.size.x, 0.1f);
	}
#endif // _DEBUG

}

void StarPrimitive::UniqeUpdate()
{
	if ((preStar != star)) {
		star.Create(mesh.get());
	}
	preStar = star;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Star")) {
		ImGui::DragFloat("innerRadius", &star.innerRadius, 0.1f);
		ImGui::DragFloat("outerRadius", &star.outerRadius, 0.1f);
		ImGui::DragInt("segments_", &star.segments);
		if (star.segments <= 3) {
			star.segments = 3;
		}
	}
#endif // _DEBUG
}

void CrescentPrimitive::UniqeUpdate()
{
	if ((preCrescent != crescent)) {
		crescent.Create(mesh.get());
	}

	preCrescent = crescent;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Crescent")) {
		ImGui::DragFloat("innerRadius", &crescent.innerRadius, 0.1f);
		ImGui::DragFloat("outerRadius", &crescent.outerRadius, 0.1f);
		ImGui::DragFloat("distance", &crescent.distance);
		ImGui::DragInt("segments", &crescent.segments);
		if (crescent.segments <= 8) {
			crescent.segments = 8;
		}
		if (crescent.distance <= 0) {
			crescent.distance = 0.0001f;
		}
		if (crescent.innerRadius >= crescent.outerRadius) {
			crescent.innerRadius = crescent.outerRadius;
		}
	}
#endif // _DEBUG

}

void RingPrimitive::UniqeUpdate()
{
	if ((preRing != ring)) {
		ring.Create(mesh.get());
	}

	preRing = ring;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Ring")) {
		ImGui::DragFloat("innerRadius", &ring.innerRadius, 0.1f);
		ImGui::DragFloat("outerRadius", &ring.outerRadius, 0.1f);
		ImGui::DragInt("segments_", &ring.segments);
		if (ring.segments <= 5) {
			ring.segments = 5;
		}
	}
#endif // _DEBUG

}

void CrossPrimitive::UniqeUpdate()
{
	if ((preCross != cross)) {
		cross.Create(mesh.get());
	}

	preCross = cross;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Cross")) {
		ImGui::DragFloat("armLength", &cross.armLength, 0.1f);
		ImGui::DragFloat("armWidth", &cross.armWidth, 0.1f);
	}
#endif // _DEBUG

}

void CylinderPrimitive::UniqeUpdate()
{
	if ((preCylinder != cylinder)) {
		cylinder.Create(mesh.get());
	}

	preCylinder = cylinder;

#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Cylinder")) {
		ImGui::DragFloat("height", &cylinder.height, 0.1f);
		ImGui::DragFloat("innerRadius", &cylinder.innerRadius, 0.1f);
		ImGui::DragFloat("outerRadius", &cylinder.outerRadius, 0.1f);
		ImGui::DragInt("segments", &cylinder.segments);
		ImGui::Checkbox("isCover", &cylinder.isCover);
		if (cylinder.segments <= 3) {
			cylinder.segments = 3;
		}
	}
#endif // _DEBUG
}

void SpherePrimitive::UniqeUpdate()
{
	if ((sphere != preSphere)) {
		sphere.Create(mesh.get());
	}

	preSphere = sphere;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Sphere")) {
		ImGui::DragFloat("radius", &sphere.radius, 0.1f);
		ImGui::DragInt("latitudeSegments", &sphere.latitudeSegments);
		ImGui::DragInt("longitudeSegments", &sphere.longitudeSegments);
		ImGui::Checkbox("isTopBased", &sphere.isTopBased);
	}
#endif // _DEBUG

}

void ArrowPrimitive::UniqeUpdate()
{
	if (preArrow != arrow) {
		arrow.Create(mesh.get());
	}

	preArrow = arrow;
}

void TubePrimitive::UniqeUpdate()
{
	if ((preTube != tube)) {
		tube.Create(mesh.get());
	}

	preTube = tube;
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Tube")) {
		ImGui::DragFloat("height", &tube.height, 0.1f);
		ImGui::DragFloat("radius", &tube.radius, 0.1f);
		ImGui::DragFloat("innerRadius", &tube.innerRadius, 0.1f);
		ImGui::DragInt("segments", &tube.segments);
		if (tube.segments <= 3) {
			tube.segments = 3;
		}
	}
#endif // _DEBUG
}

void PyramidPrimitive::UniqeUpdate()
{
	if ((prePyramid != pyramid)) {
		pyramid.Create(mesh.get());
	}

	prePyramid = pyramid;

#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Pyramid")) {
		ImGui::DragFloat("height", &pyramid.height, 0.1f);
		ImGui::DragFloat("radius", &pyramid.radius, 0.1f);
		ImGui::DragInt("segments", &pyramid.segments);
		if (pyramid.segments <= 3) {
			pyramid.segments = 3;
		}
	}
#endif // _DEBUG
}

void TorusPrimitive::UniqeUpdate()
{
	if ((torus != preTorus)) {
		torus.Create(mesh.get());
	}

	preTorus = torus;

#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Torus")) {
		ImGui::DragFloat("innerRadius", &torus.innerRadius, 0.1f);
		ImGui::DragFloat("outerRadius", &torus.outerRadius, 0.1f);
		ImGui::DragInt("tubeSegments", &torus.tubeSegments);
		ImGui::DragInt("segments", &torus.segments);
		if (torus.segments <= 3) {
			torus.segments = 3;
		}
		if (torus.tubeSegments <= 3) {
			torus.tubeSegments = 3;
		}
	}
#endif // _DEBUG
}
