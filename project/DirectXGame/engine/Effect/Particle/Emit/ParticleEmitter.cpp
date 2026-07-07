
// engine
#include "ParticleEmitter.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Particle/CPU/ParticleField.h"
#include "DirectXGame/engine/Utility/RangeUtility.h"
#include "DirectXGame/engine/Math/Random.h"
#include "DirectXGame/engine/Math/MathFunctions.h"

namespace {
	constexpr float kEmitterEpsilon = 0.0001f;
	constexpr float kTwoPi = 6.28318530718f;

	float Random01(std::mt19937& randomEngine)
	{
		// 0から1の範囲でエミット位置の補間率を作る
		return Random::RandFloat(Range<float>{ 0.0f,1.0f }, randomEngine);
	}

	float ResolveDirectionSpeed(const Engine::EffectEmitData& emitData, std::mt19937& randomEngine)
	{
		// 既存の速度レンジから速さだけを取り出し、形状方向へ向け直す
		const Vector3 randomVelocity = Random::RandVector3(emitData.velocity, randomEngine);
		float speed = randomVelocity.Length();
		if (speed <= kEmitterEpsilon) {
			speed = emitData.velocity.median.Length();
		}
		if (speed <= kEmitterEpsilon) {
			speed = 1.0f;
		}
		return speed;
	}

	Vector3 BuildShapeVelocity(
		const Engine::EffectEmitData& emitData,
		EmitData::DirectionType directionType,
		std::mt19937& randomEngine,
		const Vector3& emitPos,
		const Vector3& centerPos,
		const Vector3& normal)
	{
		// 形状からの出方に応じて、ランダム速度か形状基準の速度を選ぶ
		switch (directionType)
		{
		case EmitData::DirectionType::kNormal:
			return Math::NormalizeSafe(normal, Vector3{ 0.0f,1.0f,0.0f }, kEmitterEpsilon) * ResolveDirectionSpeed(emitData, randomEngine);
		case EmitData::DirectionType::kInverse:
			return Math::NormalizeSafe(centerPos - emitPos, -Math::NormalizeSafe(normal, Vector3{ 0.0f,1.0f,0.0f }, kEmitterEpsilon), kEmitterEpsilon) * ResolveDirectionSpeed(emitData, randomEngine);
		case EmitData::DirectionType::kFixed:
			return emitData.velocity.median;
		case EmitData::DirectionType::kNone:
			return Vector3{};
		case EmitData::DirectionType::kVelocity:
		case EmitData::DirectionType::kVelocityBase:
		case EmitData::DirectionType::kRandom:
		default:
			return Random::RandVector3(emitData.velocity, randomEngine);
		}
	}

	void CreateShapeParticle(
		Engine::ParticleGroup& particleGroup,
		const Engine::EffectEmitData& emitData,
		EmitData::DirectionType directionType,
		std::mt19937& randomEngine,
		const Vector3& emitPos,
		const Vector3& centerPos,
		const Vector3& normal)
	{
		// ランダム速度以外は専用方向の速度で生成する
		if (directionType == EmitData::DirectionType::kRandom ||
			directionType == EmitData::DirectionType::kVelocity ||
			directionType == EmitData::DirectionType::kVelocityBase) {
			Engine::EmitFunction::CreateParticle(particleGroup, emitData, randomEngine, emitPos);
			return;
		}

		const Vector3 velocity = BuildShapeVelocity(emitData, directionType, randomEngine, emitPos, centerPos, normal);
		Engine::EmitFunction::CreateParticle(particleGroup, emitData, randomEngine, emitPos, velocity);
	}

	Vector3 SampleTriangleSurface(const Triangle& triangle, std::mt19937& randomEngine)
	{
		// 三角形の面内を均一に選ぶため、はみ出した重心座標を折り返す
		float u = Random01(randomEngine);
		float v = Random01(randomEngine);
		if (u + v > 1.0f) {
			u = 1.0f - u;
			v = 1.0f - v;
		}
		return triangle.vertices[0] +
			(triangle.vertices[1] - triangle.vertices[0]) * u +
			(triangle.vertices[2] - triangle.vertices[0]) * v;
	}

	Vector3 SampleTriangleEdge(const Triangle& triangle, std::mt19937& randomEngine)
	{
		// 三角形の3辺から1本を選び、その辺上を補間する
		const int edgeIndex = Random::RandomInt32_t(0, 2);
		const Vector3& start = triangle.vertices[edgeIndex];
		const Vector3& end = triangle.vertices[(edgeIndex + 1) % 3];
		return Lerp(start, end, Random01(randomEngine));
	}
}

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

	EmitFunction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), transform_.worldMat_.GetWorldPosition());
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
	Vector3 normal{};
	const Vector3 centerPos = transform_.worldMat_.GetWorldPosition();
	if (emitType_ == EmitData::EmitType::kRandom) {	// ランダム
		const Vector3 localPos = Random::RandVector3(range_, particleManager->GetRandomEngine());
		pos = centerPos + localPos;
		normal = Math::NormalizeSafe(localPos, Vector3{ 0.0f,1.0f,0.0f }, kEmitterEpsilon);
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
			normal = { 1.0f,0.0f,0.0f };
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 1: // -X面
			pos.x = min.x;
			normal = { -1.0f,0.0f,0.0f };
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 2: // +Y面
			pos.y = max.y;
			normal = { 0.0f,1.0f,0.0f };
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 3: // -Y面
			pos.y = min.y;
			normal = { 0.0f,-1.0f,0.0f };
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.z = std::uniform_real_distribution<float>(min.z, max.z)(rnd);
			break;
		case 4: // +Z面
			pos.z = max.z;
			normal = { 0.0f,0.0f,1.0f };
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			break;
		case 5: // -Z面
			pos.z = min.z;
			normal = { 0.0f,0.0f,-1.0f };
			pos.x = std::uniform_real_distribution<float>(min.x, max.x)(rnd);
			pos.y = std::uniform_real_distribution<float>(min.y, max.y)(rnd);
			break;
		}
		// ワールド座標系に変換
		pos = centerPos + pos;

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
		const Vector3 localPos = Lerp(p0, p1, t);

		pos = centerPos + localPos;
		normal = Math::NormalizeSafe(localPos, Vector3{ 0.0f,1.0f,0.0f }, kEmitterEpsilon);
	}

	CreateShapeParticle(particleGroup, emitData_, directionType_, particleManager->GetRandomEngine(), pos, centerPos, normal);
}



//
void Engine::AABBParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("AABB");
	ImGui::Separator();

	ImGui::DragFloat3("range.max", &range_.max.x, 0.1f);
	ImGui::DragFloat3("range.min", &range_.min.x, 0.1f);
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
	auto& randomEngine = particleManager->GetRandomEngine();
	const Vector3 centerPos = transform_.worldMat_.GetWorldPosition();

	Vector3 normal = Random::RandomUnitVector3(randomEngine);
	float distance = radius_;
	if (emitType_ == EmitData::EmitType::kRandom) {
		// 球内部に均一に近い分布で出す
		distance = radius_ * std::cbrt(Random01(randomEngine));
	}
	else if (emitType_ == EmitData::EmitType::kEdge) {
		// 球の辺相当としてローカルXZ平面の赤道リングから出す
		const float angle = Random01(randomEngine) * kTwoPi;
		const Vector3 localNormal = { std::cos(angle),0.0f,std::sin(angle) };
		normal = MakeRotateXYZ(transform_.rotate_).Transform(localNormal).Normalize();
	}
	Vector3 pos = centerPos + normal * distance;

	CreateShapeParticle(particleGroup, emitData_, directionType_, randomEngine, pos, centerPos, normal);
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
	auto& randomEngine = particleManager->GetRandomEngine();
	const Vector3 centerPos = transform_.worldMat_.GetWorldPosition();
	Matrix4x4 rotationMatrix = MakeRotateXYZ(transform_.rotate_);

	Vector3 localPos{};
	if (emitType_ == EmitData::EmitType::kEdge) {
		// 円周上を分割辺として選ぶ
		const float angleStep = kTwoPi / static_cast<float>(corner.segment);
		const int32_t index = Random::RandomInt32_t(0, corner.segment - 1);
		const float startAngle = static_cast<float>(index) * angleStep;
		const float endAngle = static_cast<float>((index + 1) % corner.segment) * angleStep;
		const Vector3 start = { std::cos(startAngle) * corner.radius,0.0f,std::sin(startAngle) * corner.radius };
		const Vector3 end = { std::cos(endAngle) * corner.radius,0.0f,std::sin(endAngle) * corner.radius };
		localPos = Lerp(start, end, Random01(randomEngine));
	}
	else {
		// ランダム/面は円盤内から出す
		const float angle = Random01(randomEngine) * kTwoPi;
		const float distance = corner.radius * std::sqrt(Random01(randomEngine));
		localPos = { std::cos(angle) * distance,0.0f,std::sin(angle) * distance };
	}

	const Vector3 rotatedPos = rotationMatrix.Transform(localPos);
	const Vector3 pos = centerPos + rotatedPos;
	const Vector3 normal = emitType_ == EmitData::EmitType::kEdge
		? Math::NormalizeSafe(rotatedPos, Vector3{ 0.0f,1.0f,0.0f }, kEmitterEpsilon)
		: rotationMatrix.Transform({ 0.0f,1.0f,0.0f }).Normalize();
	CreateShapeParticle(particleGroup, emitData_, directionType_, randomEngine, pos, centerPos, normal);
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

	EmitFunction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
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

	EmitFunction::CreateParticle(particleGroup, emitData_, particleManager->GetRandomEngine(), pos);
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
	const Vector3 centerPos = transform_.worldMat_.GetWorldPosition();

	Vector3 localPos{};
	if (emitType_ == EmitData::EmitType::kEdge) {
		localPos = SampleTriangleEdge(triangle_, rng);
	}
	else {
		localPos = SampleTriangleSurface(triangle_, rng);
	}

	Vector3 pos = centerPos + localPos;
	Vector3 normal = triangle_.GetNormal();
	if (emitType_ == EmitData::EmitType::kEdge) {
		normal = Math::NormalizeSafe(localPos - triangle_.GetCentroid(), normal, kEmitterEpsilon);
	}
	CreateShapeParticle(particleGroup, emitData_, directionType_, rng, pos, centerPos, normal);
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
	if (!modelMesh_) {
		return;
	}

	for (auto& tri : modelMesh_->GetTriangles()) {
		lineCommon->GetDebugLineMeshData().AddLineTriangle(tri, transform_);
	}
}
// パーティクル発生
void Engine::MeshParticleEmitter::EmitUniqe() {
	if (!modelMesh_ || modelMesh_->GetTriangles().empty()) {
		return;
	}
	ParticleGroup& particleGroup = particleManager->GetParticleGroups(particleName_);

	auto& rng = particleManager->GetRandomEngine();
	const Vector3 centerPos = transform_.worldMat_.GetWorldPosition();

	int index = static_cast<int>(Random::RandomSize_t(0, modelMesh_->GetTriangles().size() - 1));

	Triangle tri = modelMesh_->GetTriangle(index);
	Vector3 localPos{};
	if (emitType_ == EmitData::EmitType::kEdge) {
		localPos = SampleTriangleEdge(tri, rng);
	}
	else {
		localPos = SampleTriangleSurface(tri, rng);
	}

	Vector3 pos = centerPos + localPos;
	Vector3 normal = tri.GetNormal();
	if (emitType_ == EmitData::EmitType::kEdge) {
		normal = Math::NormalizeSafe(localPos - tri.GetCentroid(), normal, kEmitterEpsilon);
	}

	CreateShapeParticle(particleGroup, emitData_, directionType_, rng, pos, centerPos, normal);
}

void Engine::MeshParticleEmitter::DebugImGui()
{
	ImGui::Separator();
	ImGui::Text("Mesh");
	ImGui::Separator();

}

#pragma endregion

