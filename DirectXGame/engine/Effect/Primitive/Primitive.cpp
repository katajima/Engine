#include "Primitive.h"

#include"PrimitiveCommon.h"

void Primitive::Init(const std::string& tex, const Color color, const std::string& name)
{
	mesh = std::make_unique<Mesh>();
	MeshInitialize();

	mesh->Initialize(primitiveCommon_->GetDxCommon());

	material = std::make_unique<Material>();
	material->Initialize(primitiveCommon_->GetDxCommon());
	material->tex_.diffuseFilePath = tex;

	material->color = color;
	transfomation = std::make_unique<Transfomation>();

	transfomation->Initialize(primitiveCommon_->GetDxCommon());



	//transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	mat_.Identity();


	if (name == "") {
		name_ = "NoName";
	}
	else {
		name_ = name;
	}
}

void Primitive::Update()
{
	material->GPUData();
	//material->transform.translate.y += 0.001f;
	mat_ = MakeAffineMatrix(transform.scale, Vector3(transform.rotate), transform.translate);

#ifdef _DEBUG
	MeshUpdateImGui();
#endif // _DEBUG



	MeshUpdate();

	transfomation->Update(camera_, mat_);

}

void Primitive::MeshInitialize()
{
	switch (type_)
	{
	case Primitive::ShapeType::None:

		break;
	case Primitive::ShapeType::Plane:
		plane.Create(mesh.get());
		break;
	case Primitive::ShapeType::Triangle:
		triangle.Create(mesh.get());
		break;
	case Primitive::ShapeType::Circle:
		circle.Create(mesh.get());
		break;
	case Primitive::ShapeType::Star:
		star.Create(mesh.get());
		break;
	case Primitive::ShapeType::Crescent:
		crescent.Create(mesh.get());
		break;
	case Primitive::ShapeType::Ring:
		ring.Create(mesh.get());
		break;
	case Primitive::ShapeType::Arrow:
		arrow.Create(mesh.get());
		break;
	case Primitive::ShapeType::Cross:
		cross_.Create(mesh.get());
		break;
	case Primitive::ShapeType::Cube:
		cube.Create(mesh.get());
		break;
	case Primitive::ShapeType::Sphere:
		sphere.Create(mesh.get());
		break;
	case Primitive::ShapeType::Cylinder:
		cylinder.Create(mesh.get());
		break;
	case Primitive::ShapeType::Tube:
		tube.Create(mesh.get());
		break;
	case Primitive::ShapeType::Pyramid:
		pyramid.Create(mesh.get());
		break;
	case Primitive::ShapeType::Torus:
		torus.Create(mesh.get());
		break;
	case Primitive::ShapeType::Spring:

		break;
	default:
		break;
	}
}

void Primitive::MeshUpdate()
{


	switch (type_)
	{
	case Primitive::ShapeType::None:
		break;
	case Primitive::ShapeType::Plane:
		break;
	case Primitive::ShapeType::Triangle:
		if (triangle != oTriangle) {
			triangle.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Circle:
		if (oCircle != circle) {
			circle.Create(mesh.get());	
		}
		break;
	case Primitive::ShapeType::Star:
		if ((oStar != star)) {
			star.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Crescent:
		if ((oCrescent != crescent)) {
			crescent.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Ring:
		if ((oRing != ring)) {
			ring.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Arrow:
		if (oArrow != arrow) {
			arrow.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Cross:
		if ((oCross_ != cross_)) {
			cross_.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Cube:
		if (oCube != cube) {
			cube.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Sphere:
		if ((sphere != oSphere)) {
			sphere.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Cylinder:
		if ((oCylinder != cylinder)) {
			cylinder.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Tube:
		if ((oTube != tube)) {
			tube.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Pyramid:
		if ((oPyramid != pyramid)) {
			pyramid.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Torus:
		if ((torus != oTorus)) {
			torus.Create(mesh.get());
		}
		break;
	case Primitive::ShapeType::Spring:

		break;
	default:
		break;
	}

	oCube = cube;
	oTorus = torus;
	oCross_ = cross_;
	oStar = star;
	oCrescent = crescent;
	oRing = ring;
	oSphere = sphere;
	oCylinder = cylinder;
	oPyramid = pyramid;
	oCircle = circle;
	oArrow = arrow;
}

void Primitive::MeshUpdateImGui()
{
	ImGui::Begin("Primitive");
	if (ImGui::CollapsingHeader(name_.c_str())) {
		std::string str = name_ + "_translate";
		ImGui::DragFloat3(str.c_str(), &transform.translate.x, 0.1f);
		str = name_ + "_rotate";
		ImGui::DragFloat3(str.c_str(), &transform.rotate.x, 0.01f);
		str = name_ + "_scale";
		ImGui::DragFloat3(str.c_str(), &transform.scale.x, 0.1f);
		int i = (int)mesh->vertices.size();
		ImGui::InputInt("index2", &i);
		str = name_ + "material";
		ImGui::DragFloat3(str.c_str(), &material->transform.scale.x, 0.01f);
		str += "rotate";
		ImGui::DragFloat3(str.c_str(), &material->transform.rotate.x, 0.01f);
		//material->transform.rotate.x += 1.0f;

		ImGui::Checkbox("isScaleX", &aimetion_.isScaleX);
		ImGui::Checkbox("isScaleY", &aimetion_.isScaleY);
		ImGui::DragFloat2("speed", &aimetion_.speed.x, 0.01f);
		ImGui::DragFloat2("maxCount", &aimetion_.maxCount.x, 0.01f);
		
		ImGui::Checkbox("isRotateX", &aimetion_.isRotateX);
		ImGui::Checkbox("isRotateY", &aimetion_.isRotateY);
		ImGui::DragFloat2("rotateSpeed", &aimetion_.rotateSpeed.x, 0.01f);
		ImGui::DragFloat2("maxRotate", &aimetion_.maxRotate.x, 0.01f);
		
		if (aimetion_.isScaleX) {
			material->transform.scale.x += aimetion_.speed.x;
			if (material->transform.scale.x >= aimetion_.maxCount.x) {
				material->transform.scale.x = 0;
			}

		}
		if (aimetion_.isScaleY) {
			material->transform.scale.y += aimetion_.speed.y;
			if (material->transform.scale.y >= aimetion_.maxCount.y) {
				material->transform.scale.y = 0;
			}
		}

		if (aimetion_.isRotateX) {
			material->transform.rotate.x += aimetion_.rotateSpeed.x;
			if (material->transform.rotate.x >= aimetion_.maxRotate.x) {
				material->transform.rotate.x = 0;
			}

		}
		if (aimetion_.isRotateY) {
			material->transform.rotate.y += aimetion_.rotateSpeed.y;
			if (material->transform.rotate.y >= aimetion_.maxRotate.y) {
				material->transform.rotate.y = 0;
			}
		}


		switch (type_)
		{
		case Primitive::ShapeType::None:
			break;
		case Primitive::ShapeType::Plane:
			break;
		case Primitive::ShapeType::Triangle:
			break;
		case Primitive::ShapeType::Circle:
			if (ImGui::CollapsingHeader("Circle")) {
				ImGui::DragInt("segments", &circle.segments);
				ImGui::DragFloat("radius", &circle.radius, 0.1f);
			}
			break;
		case Primitive::ShapeType::Star:
			if (ImGui::CollapsingHeader("Star")) {
				ImGui::DragFloat("innerRadius", &star.innerRadius, 0.1f);
				ImGui::DragFloat("outerRadius", &star.outerRadius, 0.1f);
				ImGui::DragInt("segments_", &star.segments);
				if (star.segments <= 3) {
					star.segments = 3;
				}
			}

			break;
		case Primitive::ShapeType::Crescent:
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

			break;
		case Primitive::ShapeType::Ring:
			if (ImGui::CollapsingHeader("Ring")) {
				ImGui::DragFloat("innerRadius", &ring.innerRadius, 0.1f);
				ImGui::DragFloat("outerRadius", &ring.outerRadius, 0.1f);
				ImGui::DragInt("segments_", &ring.segments);
				if (ring.segments <= 5) {
					ring.segments = 5;
				}
			}

			break;
		case Primitive::ShapeType::Arrow:

			break;
		case Primitive::ShapeType::Cross:

			if (ImGui::CollapsingHeader("Cross")) {
				ImGui::DragFloat("armLength", &cross_.armLength, 0.1f);
				ImGui::DragFloat("armWidth", &cross_.armWidth, 0.1f);
			}

			break;
		case Primitive::ShapeType::Cube:
			if (ImGui::CollapsingHeader("Cube")) {
				ImGui::DragFloat3("size", &cube.size.x, 0.1f);
			}
			break;
		case Primitive::ShapeType::Sphere:
			if (ImGui::CollapsingHeader("Sphere")) {
			ImGui::DragFloat("radius", &sphere.radius, 0.1f);
			ImGui::DragInt("latitudeSegments", &sphere.latitudeSegments);
			ImGui::DragInt("longitudeSegments", &sphere.longitudeSegments);
			ImGui::Checkbox("isTopBased", &sphere.isTopBased);
			}

			break;
		case Primitive::ShapeType::Cylinder:
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

			break;
		case Primitive::ShapeType::Tube:
			if (ImGui::CollapsingHeader("Tube")) {
				ImGui::DragFloat("height", &tube.height, 0.1f);
				ImGui::DragFloat("radius", &tube.radius, 0.1f);
				ImGui::DragFloat("innerRadius", &tube.innerRadius, 0.1f);
				ImGui::DragInt("segments", &tube.segments);
				if (tube.segments <= 3) {
					tube.segments = 3;
				}


			}

			break;
		case Primitive::ShapeType::Pyramid:
			if (ImGui::CollapsingHeader("Pyramid")) {
				ImGui::DragFloat("height", &pyramid.height, 0.1f);
				ImGui::DragFloat("radius", &pyramid.radius, 0.1f);
				ImGui::DragInt("segments", &pyramid.segments);
				if (pyramid.segments <= 3) {
					pyramid.segments = 3;
				}
			}

			break;
		case Primitive::ShapeType::Torus:
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

			break;
		case Primitive::ShapeType::Spring:
			oSpring = spring;

			if (ImGui::CollapsingHeader("Spring")) {
				ImGui::DragFloat("height", &spring.height, 0.1f);
				ImGui::DragFloat("width", &spring.width, 0.1f);
				ImGui::DragFloat("spacing", &spring.spacing, 0.1f);
				ImGui::DragFloat("length", &spring.length, 0.1f);
				ImGui::DragInt("turns", &spring.turns, 0.1f);
				ImGui::DragFloat("thickness", &spring.thickness, 0.1f);
				ImGui::DragInt("segments", &spring.segments);
				if (spring.segments <= 3) {
					spring.segments = 3;
				}

			}
			break;
		default:
			break;
		}
	}
	ImGui::End();
}

void Primitive::Draw(PsoType type)
{
	switch (type)
	{
	case Primitive::PsoType::kDefalt:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kDefalt);
		break;
	case Primitive::PsoType::kRingClamp:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kRingClamp);
		break;
	case Primitive::PsoType::kNoCull:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kNoCull);
		break;
	case Primitive::PsoType::kNoCullRingClamp:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kNoCullRingClamp);
		break;
	case Primitive::PsoType::kNoCullWireFrame:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kNoCullWireFrame);
		break;
	default:
		primitiveCommon_->DrawCommonSetting(PrimitiveCommon::PsoType::kDefalt);
		break;
	}


	if (mesh->vertices.size() != 0) {


		transfomation->GetCommandList(1);

		material->GetCommandListMaterial(0);

		material->GetCommandListTexture(2, 7, 8);



		mesh->GetCommandList();

		primitiveCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);
	}
}

void Primitive::SetStar(ShapeParameter::Star& _star)
{
	star = _star;
}