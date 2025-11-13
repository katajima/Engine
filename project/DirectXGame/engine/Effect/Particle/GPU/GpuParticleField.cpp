#include "GpuParticleField.h"
#include"DirectXGame/engine/Line/LineCommon.h"



void GpuParticleField::Init(DirectXCommon* dxCommon, LineCommon* lineCommon, std::string name)
{
	dxCommon_ = dxCommon;
	name_ = name;
	lineCommon_ = lineCommon;

	// 影響
	cbEffectFieldResource_.CreateBuffer(dxCommon_, 1);
	cbEffectFieldResource_.Data()->force = -0.01f;
	cbEffectFieldResource_.Data()->translate = { 0.0f,10.0f,0.0f };
	cbEffectFieldResource_.Data()->isEffect = 0;
	cbEffectFieldResource_.Data()->range = { 30.0f,30.0f,30.0f };
}

void GpuParticleField::UpdateImgui() 
{
	if (ImGui::TreeNode(name_.c_str())) {


		ImGui::DragFloat3("transform", &cbEffectFieldResource_.Data()->translate.x, 0.01f);
		ImGui::DragFloat3("range", &cbEffectFieldResource_.Data()->range.x, 0.01f);
		ImGui::DragFloat("scaleRange", &cbEffectFieldResource_.Data()->force, 0.01f);
		bool is  = static_cast<bool>(cbEffectFieldResource_.Data()->isEffect);
		ImGui::Checkbox("isEffect", &is);
		cbEffectFieldResource_.Data()->isEffect = static_cast<uint32_t>(is);
		ImGui::TreePop();
	}

	lineCommon_->GetDebugLineMeshData().AddLineAABB({ -cbEffectFieldResource_.Data()->range,cbEffectFieldResource_.Data()->range }, cbEffectFieldResource_.Data()->translate);
}

void GpuParticleField::Update() 
{
	cbEffectFieldResource_.SetComputeRootConstantBufferView(5); // Field影響
}