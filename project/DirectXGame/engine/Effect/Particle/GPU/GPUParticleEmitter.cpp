#include "GPUParticleEmitter.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "GpuParticleSystem.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>


#pragma region Common

void Engine::BaseGpuParticleEmitter::Init(DirectXCommon* dxCommon, LineCommon* lineCommon, GpuParticleGroup* group, std::string name)
{
	this->dxCommon = dxCommon;		// DX共通クラス
	this->group = group;				// グループ
	this->name_ = name;				// 名前
	this->lineCommon = lineCommon;	// ライン共通クラス

	// 共通データ
	cbEmitterCommon_.translate = Vector3(0.0f, 0.0f, 0.0f);
	cbEmitterCommon_.prevTranslate = cbEmitterCommon_.translate;
	cbEmitterCommon_.count = 10;
	cbEmitterCommon_.frequency = 0.00f;
	cbEmitterCommon_.frequencyTime = 0.0f;
	cbEmitterCommon_.emit = 0;
	cbEmitterCommon_.color = { 1.0f,0.8f ,0.1f };
	cbEmitterCommon_.colorRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.lifeTime = 5.0f;
	cbEmitterCommon_.velocity = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.velocityRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.scale = { 0.01f,0.01f,0.01f };
	cbEmitterCommon_.scaleRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.spawnShape = ParticleSpawnShape::Volume;
	cbEmitterCommon_.directionType = ParticleDireccion::Random;
	cbEmitterCommon_.force = 1.0f;
	cbEmitterCommon_.isAlhpa = true;
	cbEmitterCommon_.isScaling = false;
	cbEmitterCommon_.scaleAmount = 0.1f;
	cbEmitterCommon_.isGravity = false;
	cbEmitterCommon_.rotate = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.rotateRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.useBillboard = true;
	cbEmitterCommon_.particleMaxCount = 10240;

	// トレイル
	cbEmitterTrail_.trailcolor = { 1.0f,1.0f ,1.0f };
	cbEmitterTrail_.trailLifeTime = 0.5f;
	cbEmitterTrail_.isTrail = false;
	cbEmitterTrail_.trailWidth = 1.0f;


	interval_ = ConvertUtility::FramesToSeconds(2);

	// ワールド座標の初期化
	worldTransform_.Initialize();


	// 派生クラス固有の初期化
	InitUniqe();
}

void Engine::BaseGpuParticleEmitter::UpdateImGui()
{
#ifdef _DEBUG
	if (ImGui::TreeNode(name_.c_str())) {
		ImGui::Separator();
		ImGui::Text(group->GetName().c_str());
		int instance = group->GetMaxInstance();
		ImGui::InputInt("MaxInstance", &instance);
		ImGui::Separator();
		ImGui::Checkbox("isEmtter", &isEmitte_);
		if (type_ != EmitterType::Point) {
			ImGui::Separator();
			ImGui::Text("SpawnShape");
			ImGui::Separator();
			if (ImGui::Button("Volume")) {
				cbEmitterCommon_.spawnShape = ParticleSpawnShape::Volume;
			}
			if (ImGui::Button("Surface")) {
				cbEmitterCommon_.spawnShape = ParticleSpawnShape::Surface;
			}
			if (type_ != EmitterType::Sphere) {
				if (ImGui::Button("Edge")) {
					cbEmitterCommon_.spawnShape = ParticleSpawnShape::Edge;
				}
			}
			ImGui::Separator();
			ImGui::Text("SpawnDirecction");
			ImGui::Separator();
			if (ImGui::Button("Random")) {
				cbEmitterCommon_.directionType = ParticleDireccion::Random;
			}
			if (ImGui::Button("Outward")) {
				cbEmitterCommon_.directionType = ParticleDireccion::Outward;
			}
			if (ImGui::Button("Inward")) {
				cbEmitterCommon_.directionType = ParticleDireccion::Inward;
			}
		}
		ImGui::DragFloat3("transform", &worldTransform_.translate_.x, 0.01f);
		ImGui::InputFloat3("prevTranslate", &cbEmitterCommon_.prevTranslate.x);

		if (cbEmitterCommon_.directionType == ParticleDireccion::Random) {
			ImGui::DragFloat3("velocity", &cbEmitterCommon_.velocity.x, 0.01f);
			ImGui::DragFloat3("velocityRange", &cbEmitterCommon_.velocityRange.x, 0.01f);
		}
		else {
			ImGui::DragFloat("force", &cbEmitterCommon_.force, 0.01f);
		}
		ImGui::Separator();
		bool useBillboard = ConvertUtility::ToBool(cbEmitterCommon_.useBillboard);
		ImGui::Checkbox("useBillboard", &useBillboard);
		cbEmitterCommon_.useBillboard = ConvertUtility::ToUint32(useBillboard);



		bool isAlpha = ConvertUtility::ToBool(cbEmitterCommon_.isAlhpa);
		ImGui::Checkbox("isAlpha", &isAlpha);
		cbEmitterCommon_.isAlhpa = ConvertUtility::ToUint32(isAlpha);


		bool isScaling = ConvertUtility::ToBool(cbEmitterCommon_.isScaling);
		ImGui::Checkbox("isScaling", &isScaling);
		cbEmitterCommon_.isScaling = ConvertUtility::ToUint32(isScaling);
		ImGui::DragFloat("scaleAmount", &cbEmitterCommon_.scaleAmount, 0.01f);

		bool isGravity = ConvertUtility::ToBool(cbEmitterCommon_.isGravity);
		ImGui::Checkbox("isGravity", &isGravity);
		cbEmitterCommon_.isGravity = ConvertUtility::ToUint32(isGravity);



		ImGui::DragFloat3("scale", &cbEmitterCommon_.scale.x, 0.01f);
		ImGui::DragFloat3("scaleRange", &cbEmitterCommon_.scaleRange.x, 0.01f);

		ImGui::DragFloat3("rotate", &cbEmitterCommon_.rotate.x, 0.01f);
		ImGui::DragFloat3("rotateRange", &cbEmitterCommon_.rotateRange.x, 0.01f);


		ImGui::ColorEdit3("color", &cbEmitterCommon_.color.x);
		ImGui::ColorEdit3("colorRange", &cbEmitterCommon_.colorRange.x);
		ImGui::DragFloat("frequency", &cbEmitterCommon_.frequency, 0.01f);
		ImGui::DragFloat("lifeTime", &cbEmitterCommon_.lifeTime, 0.01f);
		ImGui::DragFloat("lifeTimeRange", &cbEmitterCommon_.lifeTimeRange, 0.01f);

		int count = ConvertUtility::ToInt(cbEmitterCommon_.count);;
		ImGui::DragInt("count", &count);
		if (count <= 1) {
			count = 1;
		}
		cbEmitterCommon_.count = ConvertUtility::ToUint32(count);

		ImGui::Separator();
		ImGui::Text("Trail");
		ImGui::Separator();
		bool isTrail = ConvertUtility::ToBool(cbEmitterTrail_.isTrail);
		ImGui::Checkbox("isTrail", &isTrail);
		cbEmitterTrail_.isTrail = ConvertUtility::ToUint32(isTrail);

		ImGui::DragFloat("trailLifeTime", &cbEmitterTrail_.trailLifeTime, 0.01f);
		ImGui::DragFloat("trailWidth", &cbEmitterTrail_.trailWidth, 0.01f);
		ImGui::ColorEdit3("trailcolor", &cbEmitterTrail_.trailcolor.x);



		ImGui::DragFloat("interval", &interval_, 0.01f);
		ImGui::Separator();
		ImGui::Text("Image");
		ImGui::Separator();


		ImTextureID imguiTexture = (ImTextureID)(dxCommon->GetTextureManager()->GetSrvHandleGPU(group->GetTextureName()).ptr);
		ImGui::Image(imguiTexture, ImVec2(100, 100));
		// 派生クラス固有の更新
		UpdateImGuiUniqe();
		ImGui::TreePop();
	}

	// エミッター形状の描画
	DrawLine();
#endif // _DEBUG
}

void Engine::BaseGpuParticleEmitter::Update(float deltaTime)
{
	// 加算
	cbEmitterCommon_.frequencyTime += deltaTime;
	cbEmitterCommon_.translate = worldTransform_.translate_;


	// 射出間隔を上回ったら射出許可を出して時間を調整
	if (cbEmitterCommon_.frequency <= cbEmitterCommon_.frequencyTime) {
		cbEmitterCommon_.frequencyTime -= cbEmitterCommon_.frequency;
		cbEmitterCommon_.emit = 1;

	}
	else {
		// 射出間隔を上回っていないので、射出許可は出せない
		cbEmitterCommon_.emit = 0;
	}

	if (!isEmitte_) {
		cbEmitterCommon_.emit = 0;
	}

	// グループがあるなら
	if (group) {
		// 派生クラス固有の更新
		UpdateUniqe(deltaTime);
		int count = int(cbEmitterCommon_.count);
		int threadGroupCount = (count + count_ - 1) / count_;
	}
	worldTransform_.Update();
}

#pragma endregion // 共通処理


#pragma region Sphere

void Engine::GpuParticleEmitterSphere::InitUniqe()
{
	// 球エミッター
	cbEmitterCommon_.sphereRadius = 1.0f;

	type_ = EmitterType::Sphere;

	cbEmitterCommon_.shapeType = type_;
}

void Engine::GpuParticleEmitterSphere::UpdateUniqe(float deltaTime)
{
}

void Engine::GpuParticleEmitterSphere::UpdateImGuiUniqe()
{
	ImGui::Separator();
	ImGui::Text("Sphere");
	ImGui::Separator();
	ImGui::DragFloat("radius", &cbEmitterCommon_.sphereRadius);
}

void Engine::GpuParticleEmitterSphere::DrawLine()
{
	Sphere sphere = { cbEmitterCommon_.translate,cbEmitterCommon_.sphereRadius };

	lineCommon->GetDebugLineMeshData().AddLineSphere(sphere, { 1,1,1,1 }, 16, 16);
}
#pragma endregion


#pragma region AABB
void Engine::GpuParticleEmitterAABB::InitUniqe()
{
	// 球エミッター
	cbEmitterCommon_.size = Vector3{ 1.0f ,1.0f,1.0f };

	type_ = EmitterType::AABB;


	cbEmitterCommon_.shapeType = type_;
}

void Engine::GpuParticleEmitterAABB::UpdateUniqe(float deltaTime)
{
	//cbEmitterAABB_.SetComputeRootConstantBufferView(1);		// エミッター
}

void Engine::GpuParticleEmitterAABB::UpdateImGuiUniqe()
{
	ImGui::Separator();
	ImGui::Text("AABB");
	ImGui::Separator();
	ImGui::DragFloat3("aabbSize", &cbEmitterCommon_.size.x);
}

void Engine::GpuParticleEmitterAABB::DrawLine()
{

	AABB aabb = { cbEmitterCommon_.translate - cbEmitterCommon_.size,cbEmitterCommon_.translate + cbEmitterCommon_.size };

	lineCommon->GetDebugLineMeshData().AddLineAABB(aabb, cbEmitterCommon_.translate);
}
#pragma endregion


#pragma region Point
void Engine::GpuParticleEmitterPoint::InitUniqe()
{
	// 球エミッター
	
	type_ = EmitterType::Point;
	cbEmitterCommon_.shapeType = type_;
}

void Engine::GpuParticleEmitterPoint::UpdateUniqe(float deltaTime)
{
	//cbEmitterPoint_.SetComputeRootConstantBufferView(1);		// エミッター
}

void Engine::GpuParticleEmitterPoint::UpdateImGuiUniqe()
{
	ImGui::Separator();
	ImGui::Text("Point");
	ImGui::Separator();
	
	ImGui::Separator();
	ImGui::Text("Interpolation");
	ImGui::Separator();
	if (ImGui::Button("NoUse")) {
		cbEmitterCommon_.interpolation = EmitterInterpolation::NoUse;
	}
	if (ImGui::Button("Random")) {
		cbEmitterCommon_.interpolation = EmitterInterpolation::Random;
	}
	if (ImGui::Button("Sequential")) {
		cbEmitterCommon_.interpolation = EmitterInterpolation::Sequential;
	}
}

void Engine::GpuParticleEmitterPoint::DrawLine()
{
}
#pragma endregion




