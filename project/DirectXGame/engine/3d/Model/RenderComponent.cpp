#include "RenderComponent.h"


#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"

#include "DirectXGame/engine/3d/Model/ModelCommon.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"



void RenderComponent::Init(Entity3DManager* entity3DManager, ObjectModelType objectType, PSOType rasterizerType)
{
	entity3DManager_ = entity3DManager;
	objectType_ = objectType;
	rasterizerType_ = rasterizerType;

	isSkin_ = false;

	switch (objectType)
	{
	case ObjectModelType::kNormal:
		objectTypeName = "NormalModelObject";
		break;
	case ObjectModelType::kAnimation:
		objectTypeName = "AnimationModelObject";
		break;
	case ObjectModelType::kSkinning:
		objectTypeName = "SkinningModelObject";
		break;
	case ObjectModelType::kPrimitive:
		objectTypeName = "PrimitiveObject";
		break;
	case ObjectModelType::kSkyBox:
		objectTypeName = "SkyBoxObject";
		break;
	case ObjectModelType::kOcean:
		objectTypeName = "OceanObject";
		break;
	default:
		objectTypeName = "NoObject";
		break;
	}

}

void RenderComponent::Update()
{
	if (model || primitive_ || skyBox_ || ocean_) {
		isSkin_ = true;
	}
	else {
		isSkin_ = false;
	}

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
		if (model) {
			std::vector<ModelMesh*> tra;
			std::vector<ModelMesh*> opa;


			for (auto& mesh : model->modelData.mesh) {

				if (mesh->material->alpha_ < 1.0f) {
					tra.push_back(mesh.get());
				}
				else {
					opa.push_back(mesh.get());
				}
			}

			for (auto& mesh : opa) {
				if (mesh->material->alpha_ < 1.0f) {
					ObjectNormalTypeDiscrimination(PSOType::Transparent);
				}
				else {
					ObjectNormalTypeDiscrimination(rasterizerType_);
				}


				DrawSetting();

				mesh->material->GetCommandListMaterial(0);

				mesh->material->GetCommandListTexture(2, 7, 8);

				mesh->GetCommandList();

				// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
				entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetModelManager()->
					GetModelCommon()->GetCommand()->GetList()->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);
			}

			for (auto& mesh : tra) {
				if (mesh->material->alpha_ < 1.0f) {
					ObjectNormalTypeDiscrimination(PSOType::Transparent);
				}
				else {
					ObjectNormalTypeDiscrimination(rasterizerType_);
				}


				DrawSetting();

				mesh->material->GetCommandListMaterial(0);

				mesh->material->GetCommandListTexture(2, 7, 8);

				mesh->GetCommandList();

				// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
				entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetModelManager()->
					GetModelCommon()->GetCommand()->GetList()->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);
			}





			for (auto& mesh : model->modelData.mesh)
			{

			}
			//model->Draw();
		}
		break;
	case ObjectModelType::kAnimation:
		if (model) {
			ObjectNormalTypeDiscrimination(rasterizerType_);

			DrawSetting();


			// 3Dモデルが割り当てれていれば描画する

			model->Draw();
		}
		break;
	case ObjectModelType::kSkinning:
		if (model) {
			ObjectSkinningTypeDiscrimination(rasterizerType_);

			DrawSettingSkin();

			// 3Dモデルが割り当てれていれば描画する

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

float RenderComponent::GetAlpha()
{
	float a;
	switch (objectType_)
	{
	case ObjectModelType::kNormal:
		a = model->GetMaterialAlpha();
		break;
	case ObjectModelType::kAnimation:
		a = model->GetMaterialAlpha();
		break;
	case ObjectModelType::kSkinning:
		a = model->GetMaterialAlpha();
		break;
	case ObjectModelType::kPrimitive:
		a = primitive_->GetMaterial()->color.a;
		break;
	case ObjectModelType::kSkyBox:
		a = skyBox_->GetMaterial()->color.a;
		break;
	case ObjectModelType::kOcean:
		a = ocean_->GetMaterial()->color.a;
		break;
	default:
		a = 1.0f;
		break;
	}

	return a;
}

void RenderComponent::ObjectNormalTypeDiscrimination(PSOType type)
{
	Object3dCommon* object = entity3DManager_->GetObject3dCommon();

	object->DrawCommonSetting(type);
}

void RenderComponent::ObjectSkinningTypeDiscrimination(PSOType type)
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

	skinning->DrawCommonSetting(type);
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
