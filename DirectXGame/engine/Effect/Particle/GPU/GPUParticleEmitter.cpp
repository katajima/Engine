#include "GPUParticleEmitter.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "GpuParticleSystem.h"

void GpuParticleEmitter::Init(DirectXCommon* dxCommon, LineCommon* lineCommon, GpuParticleGroup* group,std::string name)
{
	dxCommon_ = dxCommon;
	group_ = group;
	name_ = name;
	lineCommon_ = lineCommon;
	// 球エミッター
	cbEmitterSphere_.CreateBuffer(dxCommon_, 1);
	cbEmitterSphere_.Data()->count = 200;
	cbEmitterSphere_.Data()->frequency = 0.00f;
	cbEmitterSphere_.Data()->frequencyTime = 0.0f;
	cbEmitterSphere_.Data()->translate = Vector3(0.0f, 10.0f, 0.0f);
	cbEmitterSphere_.Data()->radius = 0.25f;
	cbEmitterSphere_.Data()->emit = 0;

	cbEmitterSphere_.Data()->color = { 1.0f,0.8f ,0.1f };
	cbEmitterSphere_.Data()->colorRange = { 0.0f,0.0f,0.0f };
	cbEmitterSphere_.Data()->lifeTime = 5.0f;
	cbEmitterSphere_.Data()->velocity = { 0.0f,2.0f,0.0f };
	cbEmitterSphere_.Data()->velocityRange = { 0.5f,0.0f,0.5f };
	cbEmitterSphere_.Data()->scale = { 0.01f,0.01f,0.01f };
	cbEmitterSphere_.Data()->scaleRange = { 0.0f,0.0f,0.0f };
}


void GpuParticleEmitter::UpdateImGui() 
{
#ifdef _DEBUG
	if (ImGui::TreeNode(name_.c_str())) {
		ImGui::Separator();
		ImGui::Text(group_->GetName().c_str());
		int instance = group_->GetMaxInstance();
		ImGui::InputInt("MaxInstance", &instance);
		ImGui::Separator();
		ImGui::Checkbox("isEmtter", &isEmitte_);
		ImGui::DragFloat3("transform", &cbEmitterSphere_.Data()->translate.x, 0.01f);
		ImGui::DragFloat3("scale", &cbEmitterSphere_.Data()->scale.x, 0.01f);
		ImGui::DragFloat3("scaleRange", &cbEmitterSphere_.Data()->scaleRange.x, 0.01f);
		ImGui::DragFloat3("velocity", &cbEmitterSphere_.Data()->velocity.x, 0.01f);
		ImGui::DragFloat3("velocityRange", &cbEmitterSphere_.Data()->velocityRange.x, 0.01f);
		ImGui::ColorEdit3("color", &cbEmitterSphere_.Data()->color.x);
		ImGui::ColorEdit3("colorRange", &cbEmitterSphere_.Data()->colorRange.x);
		ImGui::DragFloat("radius", &cbEmitterSphere_.Data()->radius);
		ImGui::DragFloat("frequency", &cbEmitterSphere_.Data()->frequency, 0.01f);
		ImGui::DragFloat("lifeTime", &cbEmitterSphere_.Data()->lifeTime, 0.01f);
		ImGui::DragFloat("lifeTimeRange", &cbEmitterSphere_.Data()->lifeTimeRange, 0.01f);
		int count = int(cbEmitterSphere_.Data()->count);
		ImGui::DragInt("count", &count);
		cbEmitterSphere_.Data()->count = uint32_t(count);
		ImGui::TreePop();
	}

	Vector3 range = { cbEmitterSphere_.Data()->radius ,cbEmitterSphere_.Data()->radius ,cbEmitterSphere_.Data()->radius };

	lineCommon_->AddLineAABB({ -range,range }, cbEmitterSphere_.Data()->translate);
#endif // _DEBUG
}

void GpuParticleEmitter::Update(float deltaTime) 
{
	// 加算
	cbEmitterSphere_.Data()->frequencyTime += deltaTime;

	// 射出間隔を上回ったら射出許可を出して時間を調整
	if (cbEmitterSphere_.Data()->frequency <= cbEmitterSphere_.Data()->frequencyTime) {
		cbEmitterSphere_.Data()->frequencyTime -= cbEmitterSphere_.Data()->frequency;
		cbEmitterSphere_.Data()->emit = 1;

	}
	else {
		// 射出間隔を上回っていないので、射出許可は出せない
		cbEmitterSphere_.Data()->emit = 0;
	}


	if (!isEmitte_) {
		cbEmitterSphere_.Data()->emit = 0;
	}


	if (group_) {

		cbEmitterSphere_.SetComputeRootConstantBufferView(1);		// エミッター
		int count = int(cbEmitterSphere_.Data()->count);
		int threadGroupCount = (count + count_ - 1) / count_;
		group_->UpdateEmitte(deltaTime, threadGroupCount);
	}

}

void GpuParticleEmitter::SetParticleGroup(GpuParticleGroup* group)
{
	group_ = group;
}
