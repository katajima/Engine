#include "GPUParticleEmitter.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "GpuParticleSystem.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>


void BaseGpuParticleEmitter::Init(DirectXCommon* dxCommon, LineCommon* lineCommon, GpuParticleGroup* group, std::string name)
{
	dxCommon_ = dxCommon;
	group_ = group;
	name_ = name;
	lineCommon_ = lineCommon;

	// 共通データ
	cbEmitterCommon_.CreateBuffer(dxCommon_, 1);
	cbEmitterCommon_.Data()->translate = Vector3(0.0f, 0.0f, 0.0f);
	cbEmitterCommon_.Data()->prevTranslate = cbEmitterCommon_.Data()->translate;
	cbEmitterCommon_.Data()->count = 10;
	cbEmitterCommon_.Data()->frequency = 0.00f;
	cbEmitterCommon_.Data()->frequencyTime = 0.0f;
	cbEmitterCommon_.Data()->emit = 0;
	cbEmitterCommon_.Data()->color = { 1.0f,0.8f ,0.1f };
	cbEmitterCommon_.Data()->colorRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->lifeTime = 5.0f;
	cbEmitterCommon_.Data()->velocity = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->velocityRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->scale = { 0.01f,0.01f,0.01f };
	cbEmitterCommon_.Data()->scaleRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->spawnShape = ParticleSpawnShape::Volume;
	cbEmitterCommon_.Data()->directionType = ParticleDireccion::Random;
	cbEmitterCommon_.Data()->force = 1.0f;
	cbEmitterCommon_.Data()->isAlhpa = true;
	cbEmitterCommon_.Data()->isScaling = false;
	cbEmitterCommon_.Data()->scaleAmount = 0.1f;
	cbEmitterCommon_.Data()->isGravity = false;
	cbEmitterCommon_.Data()->rotate = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->rotateRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->useBillboard = true;

	// トレイル
	cbEmitterTrail_.CreateBuffer(dxCommon_, 1);
	cbEmitterTrail_.Data()->trailcolor = { 1.0f,1.0f ,1.0f };
	cbEmitterTrail_.Data()->trailLifeTime = 0.5f;
	cbEmitterTrail_.Data()->isTrail = false;
	cbEmitterTrail_.Data()->trailWidth = 1.0f;


	interval_ = ConvertUtility::FramesToSeconds(2);

	// ワールド座標の初期化
	worldTransform_.Initialize();


	// 派生クラス固有の初期化
	InitUniqe();
}

void BaseGpuParticleEmitter::UpdateImGui()
{
#ifdef _DEBUG
	if (ImGui::TreeNode(name_.c_str())) {
		ImGui::Separator();
		ImGui::Text(group_->GetName().c_str());
		int instance = group_->GetMaxInstance();
		ImGui::InputInt("MaxInstance", &instance);
		ImGui::Separator();
		ImGui::Checkbox("isEmtter", &isEmitte_);
		if (type_ != EmitterType::Point) {
			ImGui::Separator();
			ImGui::Text("SpawnShape");
			ImGui::Separator();
			if (ImGui::Button("Volume")) {
				cbEmitterCommon_.Data()->spawnShape = ParticleSpawnShape::Volume;
			}
			if (ImGui::Button("Surface")) {
				cbEmitterCommon_.Data()->spawnShape = ParticleSpawnShape::Surface;
			}
			if (type_ != EmitterType::Sphere) {
				if (ImGui::Button("Edge")) {
					cbEmitterCommon_.Data()->spawnShape = ParticleSpawnShape::Edge;
				}
			}
			ImGui::Separator();
			ImGui::Text("SpawnDirecction");
			ImGui::Separator();
			if (ImGui::Button("Random")) {
				cbEmitterCommon_.Data()->directionType = ParticleDireccion::Random;
			}
			if (ImGui::Button("Outward")) {
				cbEmitterCommon_.Data()->directionType = ParticleDireccion::Outward;
			}
			if (ImGui::Button("Inward")) {
				cbEmitterCommon_.Data()->directionType = ParticleDireccion::Inward;
			}
		}
		ImGui::DragFloat3("transform", &worldTransform_.translate_.x, 0.01f);
		ImGui::InputFloat3("prevTranslate", &cbEmitterCommon_.Data()->prevTranslate.x);

		if (cbEmitterCommon_.Data()->directionType == ParticleDireccion::Random) {
			ImGui::DragFloat3("velocity", &cbEmitterCommon_.Data()->velocity.x, 0.01f);
			ImGui::DragFloat3("velocityRange", &cbEmitterCommon_.Data()->velocityRange.x, 0.01f);
		}
		else {
			ImGui::DragFloat("force", &cbEmitterCommon_.Data()->force, 0.01f);
		}
		ImGui::Separator();
		bool useBillboard = ConvertUtility::ToBool(cbEmitterCommon_.Data()->useBillboard);
		ImGui::Checkbox("useBillboard", &useBillboard);
		cbEmitterCommon_.Data()->useBillboard = ConvertUtility::ToUint32(useBillboard);



		bool isAlpha = ConvertUtility::ToBool(cbEmitterCommon_.Data()->isAlhpa);
		ImGui::Checkbox("isAlpha", &isAlpha);
		cbEmitterCommon_.Data()->isAlhpa = ConvertUtility::ToUint32(isAlpha);


		bool isScaling = ConvertUtility::ToBool(cbEmitterCommon_.Data()->isScaling);
		ImGui::Checkbox("isScaling", &isScaling);
		cbEmitterCommon_.Data()->isScaling = ConvertUtility::ToUint32(isScaling);
		ImGui::DragFloat("scaleAmount", &cbEmitterCommon_.Data()->scaleAmount, 0.01f);

		bool isGravity = ConvertUtility::ToBool(cbEmitterCommon_.Data()->isGravity);
		ImGui::Checkbox("isGravity", &isGravity);
		cbEmitterCommon_.Data()->isGravity = ConvertUtility::ToUint32(isGravity);



		ImGui::DragFloat3("scale", &cbEmitterCommon_.Data()->scale.x, 0.01f);
		ImGui::DragFloat3("scaleRange", &cbEmitterCommon_.Data()->scaleRange.x, 0.01f);

		ImGui::DragFloat3("rotate", &cbEmitterCommon_.Data()->rotate.x, 0.01f);
		ImGui::DragFloat3("rotateRange", &cbEmitterCommon_.Data()->rotateRange.x, 0.01f);


		ImGui::ColorEdit3("color", &cbEmitterCommon_.Data()->color.x);
		ImGui::ColorEdit3("colorRange", &cbEmitterCommon_.Data()->colorRange.x);
		ImGui::DragFloat("frequency", &cbEmitterCommon_.Data()->frequency, 0.01f);
		ImGui::DragFloat("lifeTime", &cbEmitterCommon_.Data()->lifeTime, 0.01f);
		ImGui::DragFloat("lifeTimeRange", &cbEmitterCommon_.Data()->lifeTimeRange, 0.01f);

		int count = ConvertUtility::ToInt(cbEmitterCommon_.Data()->count);;
		ImGui::DragInt("count", &count);
		if (count <= 1) {
			count = 1;
		}
		cbEmitterCommon_.Data()->count = ConvertUtility::ToUint32(count);

		ImGui::Separator();
		ImGui::Text("Trail");
		ImGui::Separator();
		bool isTrail = ConvertUtility::ToBool(cbEmitterTrail_.Data()->isTrail);
		ImGui::Checkbox("isTrail", &isTrail);
		cbEmitterTrail_.Data()->isTrail = ConvertUtility::ToUint32(isTrail);

		ImGui::DragFloat("trailLifeTime", &cbEmitterTrail_.Data()->trailLifeTime, 0.01f);
		ImGui::DragFloat("trailWidth", &cbEmitterTrail_.Data()->trailWidth, 0.01f);
		ImGui::ColorEdit3("trailcolor", &cbEmitterTrail_.Data()->trailcolor.x);



		ImGui::DragFloat("interval", &interval_, 0.01f);
		ImGui::Separator();
		ImGui::Text("Image");
		ImGui::Separator();
		
		
		ImTextureID imguiTexture = (ImTextureID)(dxCommon_->GetTextureManager()->GetSrvHandleGPU(group_->GetTextureName()).ptr);
		ImGui::Image(imguiTexture, ImVec2(100, 100));
		// 派生クラス固有の更新
		UpdateImGuiUniqe();
		ImGui::TreePop();
	}

	// エミッター形状の描画
	DrawLine();
#endif // _DEBUG
}

void BaseGpuParticleEmitter::Update(float deltaTime)
{
	// 加算
	cbEmitterCommon_.Data()->frequencyTime += deltaTime;
	cbEmitterCommon_.Data()->translate = worldTransform_.translate_;


	// 射出間隔を上回ったら射出許可を出して時間を調整
	if (cbEmitterCommon_.Data()->frequency <= cbEmitterCommon_.Data()->frequencyTime) {
		cbEmitterCommon_.Data()->frequencyTime -= cbEmitterCommon_.Data()->frequency;
		cbEmitterCommon_.Data()->emit = 1;

	}
	else {
		// 射出間隔を上回っていないので、射出許可は出せない
		cbEmitterCommon_.Data()->emit = 0;
	}

	if (!isEmitte_) {
		cbEmitterCommon_.Data()->emit = 0;
	}

	if (group_) {
		// 共通
		cbEmitterCommon_.SetComputeRootConstantBufferView(6);		// エミッター
		cbEmitterTrail_.SetComputeRootConstantBufferView(7);		// トレイル
		// 派生クラス固有の更新
		UpdateUniqe(deltaTime);

		int count = int(cbEmitterCommon_.Data()->count);
		int threadGroupCount = (count + count_ - 1) / count_;
		group_->UpdateEmitte(deltaTime, threadGroupCount);
	}

	time_ += deltaTime;
	if (interval_ <= time_) {
		time_ -= interval_;
		// 前回の座標を保存
		//cbEmitterCommon_.Data()->prevTranslate = cbEmitterCommon_.Data()->translate;
	}
	worldTransform_.Update();
}

#pragma region Sphere

void GpuParticleEmitterSphere::InitUniqe()
{
	// 球エミッター
	cbEmitterSphere_.CreateBuffer(dxCommon_, 1);
	cbEmitterSphere_.Data()->radius = 1.0f;

	type_ = EmitterType::Sphere;
}

void GpuParticleEmitterSphere::UpdateUniqe(float deltaTime)
{
	cbEmitterSphere_.SetComputeRootConstantBufferView(1);		// エミッター
}

void GpuParticleEmitterSphere::UpdateImGuiUniqe()
{
	ImGui::Separator();
	ImGui::Text("Sphere");
	ImGui::Separator();
	ImGui::DragFloat("radius", &cbEmitterSphere_.Data()->radius);
}

void GpuParticleEmitterSphere::DrawLine()
{
	Vector3 range = { cbEmitterSphere_.Data()->radius ,cbEmitterSphere_.Data()->radius ,cbEmitterSphere_.Data()->radius };

	Sphere sphere = { cbEmitterCommon_.Data()->translate,cbEmitterSphere_.Data()->radius };

	lineCommon_->AddLineSphere(sphere, { 1,1,1,1 }, 16, 16);
}
#pragma endregion


#pragma region AABB
void GpuParticleEmitterAABB::InitUniqe()
{
	// 球エミッター
	cbEmitterAABB_.CreateBuffer(dxCommon_, 1);
	cbEmitterAABB_.Data()->size = Vector3{ 1.0f ,1.0f,1.0f };

	type_ = EmitterType::AABB;
}

void GpuParticleEmitterAABB::UpdateUniqe(float deltaTime)
{
	cbEmitterAABB_.SetComputeRootConstantBufferView(1);		// エミッター
}

void GpuParticleEmitterAABB::UpdateImGuiUniqe()
{
	ImGui::Separator();
	ImGui::Text("AABB");
	ImGui::Separator();
	ImGui::DragFloat3("aabbSize", &cbEmitterAABB_.Data()->size.x);
}

void GpuParticleEmitterAABB::DrawLine()
{

	AABB aabb = { cbEmitterCommon_.Data()->translate - cbEmitterAABB_.Data()->size,cbEmitterCommon_.Data()->translate + cbEmitterAABB_.Data()->size };

	lineCommon_->AddLineAABB(aabb, cbEmitterCommon_.Data()->translate);
}
#pragma endregion


#pragma region Point
void GpuParticleEmitterPoint::InitUniqe()
{
	// 球エミッター
	cbEmitterPoint_.CreateBuffer(dxCommon_, 1);

	type_ = EmitterType::Point;
}

void GpuParticleEmitterPoint::UpdateUniqe(float deltaTime)
{
	cbEmitterPoint_.SetComputeRootConstantBufferView(1);		// エミッター
}

void GpuParticleEmitterPoint::UpdateImGuiUniqe()
{
	ImGui::Separator();
	ImGui::Text("Point");
	ImGui::Separator();
	
	ImGui::Separator();
	ImGui::Text("Interpolation");
	ImGui::Separator();
	if (ImGui::Button("NoUse")) {
		cbEmitterPoint_.Data()->interpolation = EmitterInterpolation::NoUse;
	}
	if (ImGui::Button("Random")) {
		cbEmitterPoint_.Data()->interpolation = EmitterInterpolation::Random;
	}
	if (ImGui::Button("Sequential")) {
		cbEmitterPoint_.Data()->interpolation = EmitterInterpolation::Sequential;
	}
}

void GpuParticleEmitterPoint::DrawLine()
{
}
#pragma endregion




