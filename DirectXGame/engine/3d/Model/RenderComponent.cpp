#include "RenderComponent.h"


#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"


#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

void RenderComponent::Init(Entity3DManager* entity3DManager)
{
	entity3DManager_ = entity3DManager;
}

void RenderComponent::Update()
{
}

void RenderComponent::Draw()
{
	if (!isDraw) return;
	if (!isSkin_) return;

	SkyBoxCommon* skyBoxCommon = entity3DManager_->GetSkyBoxCommon();
	OceanManager* oceanManager = entity3DManager_->GetOceanManager();

	switch (objectType_)
	{
	case ObjectModelType::kNormal:
		ObjectNormalTypeDiscrimination(rasterizerType_);

		DrawSetting();


		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->Draw();
		}
		break;
	case ObjectModelType::kAnimation:
		ObjectNormalTypeDiscrimination(rasterizerType_);

		DrawSetting();


		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->Draw();
		}
		break;
	case ObjectModelType::kSkinning:
		ObjectSkinningTypeDiscrimination(rasterizerType_);

		DrawSettingSkin();

		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->DrawSkinning();
		}
		break;
	case ObjectModelType::kPrimitive:


		if (primitive_) {

			primitive_->DrawSetting(primitive_->GetPsoType());

			transfomation_->GetCommandList(1);

			primitive_->Draw();
		}
		break;

	case ObjectModelType::kSkyBox:

		if (skyBox_) {
			skyBoxCommon->DrawCommonSetting();

			transfomation_->GetCommandList(1);

			skyBox_->Draw();
		}
		break;
	case ObjectModelType::kOcean:

		if (ocean_) {
			oceanManager->DrawCommonSetting();

			DrawSettingOcean();

			ocean_->Draw();
		}
		break;
	}

}

void RenderComponent::ObjectNormalTypeDiscrimination(ObjectRasterizerType type)
{
	Object3dCommon* object = entity3DManager_->GetObject3dCommon();

	switch (type)
	{
	case ObjectRasterizerType::UvInterpolation_MODE_SOLID_BACK:
		object->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_BACK);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK:
		object->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_BACK:
		object->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		object->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case ObjectRasterizerType::UvInterpolation_MODE_SOLID_NONE:
		object->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_NONE);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE:
		object->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_NONE);
		break;
	case ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_NONE:
		object->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		object->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE);
		break;
	default:
		break;
	}
}

void RenderComponent::ObjectSkinningTypeDiscrimination(ObjectRasterizerType type)
{
	SkinningConmmon* skinning = entity3DManager_->GetSkinningConmmon();


	skinning->DrawComputeSetting();

	for (auto& mesh : model->modelData.mesh) {

		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(1, mesh->skinCluster->paletteSrvHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(2, mesh->skinCluster->inputVertexSrvHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(3, mesh->skinCluster->influenceSrvHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(4, mesh->skinCluster->outputVertexUavHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootConstantBufferView(0, mesh->skinCluster->skinningInfomation->GetGPUVirtualAddress());


		skinning->GetDxCommon()->GetCommandList()->Dispatch(UINT(mesh->vertices.size() + 1023) / 1024, 1, 1);

		// 初期状態を UAV 用に遷移させる
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mesh->skinCluster->outputVertexResource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		skinning->GetDxCommon()->GetCommandList()->ResourceBarrier(1, &barrier);
	}

	switch (type)
	{
	case ObjectRasterizerType::UvInterpolation_MODE_SOLID_BACK:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_BACK);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_BACK:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case ObjectRasterizerType::UvInterpolation_MODE_SOLID_NONE:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_NONE);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_NONE);
		break;
	case ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_NONE:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE);
		break;
	case ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		skinning->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE);
		break;
	default:
		break;
	}


}

void RenderComponent::DrawSetting()
{
	entity3DManager_->GetLightManager()->DrawLight();

	transfomation_->GetCommandList(1);
	transfomation_->GetCommandList(10);

	camera_->GetCommandList(4);
}

void RenderComponent::DrawSettingSkin()
{
	entity3DManager_->GetLightManager()->DrawLight();

	transfomation_->GetCommandList(1);

	camera_->GetCommandList(4);
}

void RenderComponent::DrawSettingOcean()
{
	entity3DManager_->GetOceanManager()->DrawCommonSetting();


	entity3DManager_->GetLightManager()->DrawLight();

	transfomation_->GetCommandList(1);
	transfomation_->GetCommandList(9);

	camera_->GetCommandList(4);
}
