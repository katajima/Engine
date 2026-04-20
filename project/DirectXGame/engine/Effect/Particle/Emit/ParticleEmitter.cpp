
// engine
#include "ParticleEmitter.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Particle/CPU/ParticleField.h"
#include "DirectXGame/engine/Utility/RangeUtility.h"
#include "DirectXGame/engine/Math/Random.h"

#pragma region Point
void Engine::PointParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);
}
// デバック線描画
void Engine::PointParticleEmitter::DrawEmitterLine() {}
// パーティクル発生
void Engine::PointParticleEmitter::EmitUniqe() {

	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);

	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), transform_.worldMat_.GetWorldPosition());
}
#pragma endregion

#pragma region AABB

void Engine::AABBParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);

	//
	range_ = { {-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f} };

	//globalVariables_->AddItem(emitName,"rangeAABB.min", range_.min);
	//globalVariables_->AddItem(emitName,"rangeAABB.max", range_.max);
	//// グローバル変数適用
	//ApplyGlobalVariablesUniqe();
}

void Engine::AABBParticleEmitter::ApplyGlobalVariablesUniqe() {
	//range_.min = globalVariables_->GetValue<Vector3>(emitName_,"rangeAABB.min");
	//range_.max = globalVariables_->GetValue<Vector3>(emitName_,"rangeAABB.max");
}

// デバック線描画
void Engine::AABBParticleEmitter::DrawEmitterLine() { lineCommon->GetDebugLineMeshData().AddLineAABB({ range_.min,range_.max }, transform_.worldMat_.GetWorldPosition(), Vector4{ 1,1,1,1 }); }
// パーティクル発生
void Engine::AABBParticleEmitter::EmitUniqe() {
	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);
	auto& rnd = particleManager->GetRandomEngine();

	ApplyGlobalVariablesUniqe();

	Vector3 pos{};
	if (emitType_ == EmitData::EmitType::kRandom) {	// ランダム
		pos = transform_.worldMat_.GetWorldPosition() + Random::RandVector3(range_, particleManager->GetRandomEngine());
	}
	else if (emitType_ == EmitData::EmitType::kSurface) { // 表面
		// AABBの最小・最大座標
		Vector3 min = range_.min;
		Vector3 max = range_.max;

		// ランダムで1面を選択
		int face = std::uniform_int_distribution<int>(0, 5)(rnd);

		switch (face) {
		case 0: // +X面
			pos.x = max.x;
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 1: // -X面
			pos.x = min.x;
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 2: // +Y面
			pos.y = max.y;
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 3: // -Y面
			pos.y = min.y;
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 4: // +Z面
			pos.z = max.z;
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			break;
		case 5: // -Z面
			pos.z = min.z;
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			break;
		}
		// ワールド座標系に変換
		pos = transform_.worldMat_.GetWorldPosition() + pos;

	}
	else if (emitType_ == EmitData::EmitType::kEdge) { // 辺
		Vector3 min = range_.min;
		Vector3 max = range_.max;

		// 12本のエッジ定義（始点・終点ペア）
		const std::pair<Vector3, Vector3> edges[12] = {
			{{min.x,min.y,min.z}, {max.x,min.y,min.z}},
			{{min.x,max.y,min.z}, {max.x,max.y,min.z}},
			{{min.x,min.y,max.z}, {max.x,min.y,max.z}},
			{{min.x,max.y,max.z}, {max.x,max.y,max.z}},

			{{min.x,min.y,min.z}, {min.x,max.y,min.z}},
			{{max.x,min.y,min.z}, {max.x,max.y,min.z}},
			{{min.x,min.y,max.z}, {min.x,max.y,max.z}},
			{{max.x,min.y,max.z}, {max.x,max.y,max.z}},

			{{min.x,min.y,min.z}, {min.x,min.y,max.z}},
			{{max.x,min.y,min.z}, {max.x,min.y,max.z}},
			{{min.x,max.y,min.z}, {min.x,max.y,max.z}},
			{{max.x,max.y,min.z}, {max.x,max.y,max.z}},
		};

		// ランダムで1本のエッジ選択
		int edgeIndex = std::uniform_int_distribution<int>(0, 11)(rnd);
		auto [p0, p1] = edges[edgeIndex];

		// 補間してランダム位置
		float t = std::uniform_real_distribution<float>(0.0f, 1.0f)(rnd);
		pos = Lerp(p0, p1, t);

		pos = transform_.worldMat_.GetWorldPosition() + pos;
	}


	Vector3 dire{};
	if (directionType_ == EmitData::DirectionType::kRandom) {
		dire = Random::RandVector3(emitData_.velocity, particleManager->GetRandomEngine());
	}
	else if (directionType_ == EmitData::DirectionType::kNormal) {

	}


	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
}



//
void Engine::AABBParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("AABB");
	ImGui::Separator();

	ImGui::DragFloat3("renge.max", &range_.max.x, 0.1f);
	ImGui::DragFloat3("renge.min", &range_.min.x, 0.1f);
}
// 範囲設定
void Engine::AABBParticleEmitter::SetRange(Vector3 min, Vector3 max)
{
	range_.min = min;
	range_.max = max;
}

#pragma endregion

#pragma region Shpere

void Engine::SphereParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);

	// 半径
	radius_ = 1.0f;

	//globalVariables_->AddItem(emitName, "radiusShpere", radius_);
	//// グローバル変数適用
	//ApplyGlobalVariablesUniqe();
}

void Engine::SphereParticleEmitter::ApplyGlobalVariablesUniqe() {
	//radius_ = globalVariables_->GetValue<float>(emitName_, "radiusShpere");
}

// デバック線描画
void Engine::SphereParticleEmitter::DrawEmitterLine() {
	Sphere sphere{};

	sphere.radius = radius_;
	sphere.center = transform_.worldMat_.GetWorldPosition();

	lineCommon->GetDebugLineMeshData().AddLineSphere(sphere, Vector4{ 1,1,1,1 });
}
// パーティクル発生
void Engine::SphereParticleEmitter::EmitUniqe() {
	ApplyGlobalVariablesUniqe();

	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);

	Vector3 pos = transform_.worldMat_.GetWorldPosition() + Random::RandomUnitVector3(particleManager->GetRandomEngine()) * radius_;

	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
}
//
void Engine::SphereParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("Sphere");
	ImGui::Separator();

	ImGui::DragFloat("radius", &radius_, 0.1f);
}

#pragma endregion

#pragma region Circle

void Engine::CornerParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);

	//
	corner.segment = 8;
	corner.radius = 1.0f;
	corner.center = {};

	//globalVariables_->AddItem(emitName, "corner.segment", corner.segment);
	//globalVariables_->AddItem(emitName, "corner.radius", corner.radius);

	//// グローバル変数適用
	//ApplyGlobalVariablesUniqe();
}

void Engine::CornerParticleEmitter::ApplyGlobalVariablesUniqe() {
	/*corner.segment = globalVariables_->GetValue<int>(emitName_, "corner.segment");
	corner.radius = globalVariables_->GetValue<float>(emitName_, "radiusShpere");*/

}

void Engine::CornerParticleEmitter::UpdateUniqe() {
	ApplyGlobalVariablesUniqe();
	corner.center = transform_.worldMat_.GetWorldPosition();
}

// デバック線描画
void Engine::CornerParticleEmitter::DrawEmitterLine() { lineCommon->GetDebugLineMeshData().AddLineCorner(corner, transform_); }
// パーティクル発生
void Engine::CornerParticleEmitter::EmitUniqe() {
	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);
	float angleStep;
	std::vector<Vector3> vertices;
	// 頂点を計算
	angleStep = DirectX::XM_2PI / corner.segment; // 360° を segment 分割

	// 回転行列を作成
	Matrix4x4 rotationMatrix = MakeRotateXYZ(transform_.rotate_);
	for (int i = 0; i < corner.segment; ++i)
	{
		float angle = i * angleStep; // 各頂点の角度
		Vector3 localVertex;
		localVertex.x = cos(angle) * corner.radius;
		localVertex.y = 0.0f;
		localVertex.z = sin(angle) * corner.radius;

		// 回転を適用
		Vector3 rotatedVertex = rotationMatrix.Transform(localVertex);

		// ワールド座標へ変換
		Vector3 worldVertex = rotatedVertex;
		vertices.push_back(worldVertex);
	}
	int32_t index = Random::RandomInt32_t(0, corner.segment - 1);

	Vector3 pos = Lerp(vertices[index], vertices[(index + 1) % corner.segment], Random::RandFloat(Range{ 0.0f,1.0f }, particleManager->GetRandomEngine()))
		+ transform_.worldMat_.GetWorldPosition();


	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
}

void Engine::CornerParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("Circle");
	ImGui::Separator();

	ImGui::DragFloat("corner.radius", &corner.radius, 0.1f);
	ImGui::SliderInt("corner.segment", &corner.segment, 3, 36);
	if (corner.segment < 3) {
		corner.segment = 3;
	}
	if (ImGui::Button("segment_3")) {
		corner.segment = 3;
	}
	if (ImGui::Button("segment_4")) {
		corner.segment = 4;
	}
	if (ImGui::Button("segment_5")) {
		corner.segment = 5;
	}
	if (ImGui::Button("segment_16")) {
		corner.segment = 16;
	}
}

#pragma endregion

#pragma region Line

void Engine::LineParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);

	segment_.origin = { 0,0,0 };
	segment_.end = { 1.0f,0,0 };

	//globalVariables_->AddItem(emitName, "segment.origin", segment_.origin);
	//globalVariables_->AddItem(emitName, "segment.ends", segment_.end);

	//// グローバル変数適用
	//ApplyGlobalVariablesUniqe();
}

void Engine::LineParticleEmitter::ApplyGlobalVariablesUniqe() {
	/*segment_.origin = globalVariables_->GetValue<Vector3>(emitName_, "segment.origin");
	segment_.end = globalVariables_->GetValue<Vector3>(emitName_, "segment.ends");*/

}
// デバック線描画
void Engine::LineParticleEmitter::DrawEmitterLine() {
	Vector3 str = segment_.origin + transform_.worldMat_.GetWorldPosition();

	Vector3 end = segment_.end + transform_.worldMat_.GetWorldPosition();
	lineCommon->GetDebugLineMeshData().AddLine(str, end);
}

void Engine::LineParticleEmitter::SetSegment(Vector3 origin, Vector3 end)
{
	segment_.origin = origin;
	segment_.end = end;
}
// パーティクル発生
void Engine::LineParticleEmitter::EmitUniqe() {
	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);


	float t = Random::RandFloat(Range{ 0.0f, 1.0f }, particleManager->GetRandomEngine());
	Vector3 diff = segment_.origin + segment_.diff() * t;

	Vector3 pos = transform_.worldMat_.GetWorldPosition() + diff;

	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
}

void Engine::LineParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("Line");
	ImGui::Separator();

	ImGui::DragFloat3("origin", &segment_.origin.x, 0.1f);
	ImGui::DragFloat3("end", &segment_.end.x, 0.1f);
}

#pragma endregion

#pragma region Spline

void Engine::SplineParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);
}

void Engine::SplineParticleEmitter::ApplyGlobalVariablesUniqe() {
	
}

// デバック線描画
void Engine::SplineParticleEmitter::DrawEmitterLine() {
	lineCommon->GetDebugLineMeshData().AddSpline(controlPoints, transform_);
}
// ポイントの座標指定
void Engine::SplineParticleEmitter::SetControlPos(int index, Vector3 pos)
{
	if (index < 0 || index >= controlPoints.size()) {
		return;
	}
	else {
		controlPoints[index] = pos;
	}
}
// パーティクル発生
void Engine::SplineParticleEmitter::EmitUniqe() {
	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);


	float t = Random::RandFloat(Range{ 0.0f, 1.0f }, particleManager->GetRandomEngine());
	Vector3 pos{};

	if (controlPoints.size() < 4) {
		pos = { 0,0,0 };
	}

	pos = CatmullRom(controlPoints, t) + transform_.worldMat_.GetWorldPosition();

	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
}

void Engine::SplineParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("Spline");
	ImGui::Separator();

	for (int i = 0; i < controlPoints.size(); i++) {
		std::string index = "SplinePos" + std::to_string(i);
		ImGui::DragFloat3(index.c_str(), &controlPoints[i].x, 0.1f);
	}
}

#pragma endregion

#pragma region Triangle

void Engine::TriangleParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);

	triangle_.vertices[0] = { 0,0.5f,0 };
	triangle_.vertices[1] = { 0.5f,-0.5f,0 };
	triangle_.vertices[2] = { -0.5f,-0.5f,0 };

}

void Engine::TriangleParticleEmitter::ApplyGlobalVariablesUniqe() {

}

// デバック線描画
void Engine::TriangleParticleEmitter::DrawEmitterLine() {
	lineCommon->GetDebugLineMeshData().AddLineTriangle(triangle_, transform_);
}
// パーティクル発生
void Engine::TriangleParticleEmitter::EmitUniqe() {
	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);

	auto& rng = particleManager->GetRandomEngine();

	// 三角形の頂点 (ローカル空間想定)
	Vector3 A = triangle_.vertices[0];
	Vector3 B = triangle_.vertices[1];
	Vector3 C = triangle_.vertices[2];

	// ランダム重心座標
	float u = Random::RandFloat(Range{ 0.0f, 1.0f }, rng);
	float v = Random::RandFloat(Range{ 0.0f, 1.0f }, rng);

	if (u + v > 1.0f) {
		u = 1.0f - u;
		v = 1.0f - v;
	}

	// ローカル座標での位置
	Vector3 localPos = A + (B - A) * u + (C - A) * v;

	Vector3 pos = transform_.worldMat_.GetWorldPosition() + localPos;

	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
}

void Engine::TriangleParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("Triangle");
	ImGui::Separator();
	ImGui::DragFloat3("triangle0", &triangle_.vertices[0].x, 0.1f);
	ImGui::DragFloat3("triangle2", &triangle_.vertices[1].x, 0.1f);
	ImGui::DragFloat3("triangle3", &triangle_.vertices[2].x, 0.1f);
}

#pragma endregion


#pragma region Mesh


void Engine::MeshParticleEmitter::Initialize(Engine::ParticleManager* particleManager, Engine::GlobalVariables* globalVariables, std::string emitName, std::string particleName)
{
	// 共通部分初期化
	CommonParticleInit(particleManager, globalVariables, emitName, particleName);
}

void Engine::MeshParticleEmitter::ApplyGlobalVariablesUniqe() {
	
}
// デバック線描画
void Engine::MeshParticleEmitter::DrawEmitterLine() {

	for (auto& tri : modelMesh_->GetTriangles()) {
		lineCommon->GetDebugLineMeshData().AddLineTriangle(tri, transform_);
	}
}
// パーティクル発生
void Engine::MeshParticleEmitter::EmitUniqe() {
	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);

	auto& rng = particleManager->GetRandomEngine();

	int index = static_cast<int>(Random::RandomSize_t(0, modelMesh_->GetTriangles().size() - 1));

	Triangle tri = modelMesh_->GetTriangle(index);
	// 三角形の頂点 (ローカル空間想定)
	Vector3 A = tri.vertices[0];
	Vector3 B = tri.vertices[1];
	Vector3 C = tri.vertices[2];

	// ランダム重心座標
	float u = Random::RandFloat(Range{ 0.0f, 1.0f }, rng);
	float v = Random::RandFloat(Range{ 0.0f, 1.0f }, rng);

	if (u + v > 1.0f) {
		u = 1.0f - u;
		v = 1.0f - v;
	}

	// ローカル座標での位置
	Vector3 localPos = A + (B - A) * u + (C - A) * v;

	Vector3 pos = transform_.worldMat_.GetWorldPosition() + localPos;

	EmitFanction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
}

void Engine::MeshParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("Mesh");
	ImGui::Separator();

}

#pragma endregion

