#include "RenderComponent.h"


#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"

#include "DirectXGame/engine/3d/Model/ModelCommon.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/3d/Model/ModelData.h"
#include "DirectXGame/engine/DirectX/ShadowMap/ShadowMap.h"

Engine::RenderComponent::~RenderComponent() = default;
Engine::RenderComponent::RenderComponent() = default;

void Engine::RenderComponent::Init(EntityManager* entityManager, ObjectModelType objectType, PSOType rasterizerType)
{
	this->entityManager = entityManager;	// エンティティ3d
	objectType_ = objectType;			// オブジェクトタイプ
	rasterizerType_ = rasterizerType;	// ラスタライザタイプ

	// なにか見た目があるか
	isSkin_ = false;

	// オブジェクトタイプ
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

void Engine::RenderComponent::Update()
{
	// なにかしら見た目があるなら
	if (model || primitive || skyBox || ocean) {
		isSkin_ = true;
	}
	else {
		isSkin_ = false;
	}

}

void Engine::RenderComponent::Draw()
{
	if (!isDraw) return;
	if (!isSkin_) return;

	SkyBoxCommon* skyBoxCommon = entityManager->GetSkyBoxCommon();
	OceanManager* oceanManager = entityManager->GetOceanManager();

	switch (objectType_)
	{
	case ObjectModelType::kNormal:
		if (model) {
			std::vector<ModelMesh*> tra;
			std::vector<ModelMesh*> opa;

			int i = 0;
			for (auto& mesh : model->GetModelData().mesh) {

				//mesh->material->SetGPUMaterialInstance(materialInstances_[i], cbResources_[i].get());


				if (mesh->material->GetMaterialInstance().alpha_ < 1.0f) {
					tra.push_back(mesh.get());
				}
				else {
					opa.push_back(mesh.get());
				}
				i++;
			}

			int j = 0;
			for (auto& mesh : opa) {
				if (mesh->material->GetMaterialInstance().alpha_ < 1.0f) {
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
				entityManager->GetObject3dCommon()->GetDxCommon()->GetModelManager()->
					GetModelCommon()->GetCommand()->GetList()->DrawIndexedInstanced(UINT(mesh->GetIndices().size()), 1, 0, 0, 0);
			}

			for (auto& mesh : tra) {
				if (mesh->material->GetMaterialInstance().alpha_ < 1.0f) {
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
				entityManager->GetObject3dCommon()->GetDxCommon()->GetModelManager()->
					GetModelCommon()->GetCommand()->GetList()->DrawIndexedInstanced(UINT(mesh->GetIndices().size()), 1, 0, 0, 0);
			}
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



			std::vector<ConstantBuffer<Material::DataGPU>*> datas;

			for(auto& cb : cbResources_) {
				datas.push_back(cb.get());
			}


			model->DrawSkinning(materialInstances_,datas);
		}
		break;
	case ObjectModelType::kPrimitive:
		if (primitive) {

			primitive->DrawSetting(primitive->GetPsoType());

			transformation->GetCommandList(1);

			primitive->Draw();
		}
		break;
	case ObjectModelType::kSkyBox:
		if (skyBox) {
			skyBoxCommon->DrawCommonSetting();

			transformation->GetCommandList(1);

			skyBox->Draw();
		}
		break;
	case ObjectModelType::kOcean:
		if (ocean) {
			oceanManager->DrawCommonSetting();

			DrawSettingOcean();

			ocean->Draw();
		}
		break;
	}
}

void Engine::RenderComponent::DrawShadowMap(ShadowMap* shadowMap)
{
	if (!isDraw) return;
	// シャドウマップはモデルの深度だけを書き込む。Primitive/SkyBox/Oceanはここでは対象外。
	if (!model) return;

	switch (objectType_)
	{
	case ObjectModelType::kNormal:
	case ObjectModelType::kAnimation:
		// シャドウパスでは色やテクスチャは使わず、ライト視点の深度だけを書き込む。
		entityManager->GetObject3dCommon()->DrawShadowMapSetting();
		transformation->GetCommandList(0);
		shadowMap->SetGraphicsRootConstantBufferView(1);

		for (auto& mesh : model->GetModelData().mesh) {
			if (mesh->material->GetMaterialInstance().alpha_ < 1.0f) {
				continue;
			}

			mesh->GetCommandList();
			entityManager->GetObject3dCommon()->GetDxCommon()->GetModelManager()->
				GetModelCommon()->GetCommand()->GetList()->DrawIndexedInstanced(UINT(mesh->GetIndices().size()), 1, 0, 0, 0);
		}
		break;
	case ObjectModelType::kSkinning:
		// スキニングはCSで変形済み頂点を作ってから、その頂点をシャドウマップへ描画する。
		ObjectSkinningTypeDiscrimination(PSOType::ShadowMap);
		transformation->GetCommandList(0);
		shadowMap->SetGraphicsRootConstantBufferView(1);

		for (auto& mesh : model->GetModelData().mesh) {
			if (mesh->material->GetMaterialInstance().alpha_ < 1.0f) {
				continue;
			}

			mesh->GetCommandList(mesh->skinCluster->outputBufferView, mesh->GetVertexBufferView());
			entityManager->GetObject3dCommon()->GetDxCommon()->GetModelManager()->
				GetModelCommon()->GetCommand()->GetList()->DrawIndexedInstanced(UINT(mesh->GetIndices().size()), 1, 0, 0, 0);

			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = mesh->skinCluster->outputVertexResource.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			entityManager->GetObject3dCommon()->GetDxCommon()->GetCommandList()->ResourceBarrier(1, &barrier);
		}
		break;
	default:
		break;
	}
}

float Engine::RenderComponent::GetAlpha()
{
	// モデルごとの透明度取得
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
		a = primitive->GetMaterial()->GetMaterialInstance().color.a;
		break;
	case ObjectModelType::kSkyBox:
		a = skyBox->GetMaterial()->GetMaterialInstance().color.a;
		break;
	case ObjectModelType::kOcean:
		a = ocean->GetMaterial()->GetMaterialInstance().color.a;
		break;
	default:
		a = 1.0f;
		break;
	}

	return a;
}

void Engine::RenderComponent::ObjectNormalTypeDiscrimination(PSOType type)
{
	Object3dCommon* object = entityManager->GetObject3dCommon();

	object->DrawCommonSetting(type);
}

void Engine::RenderComponent::ObjectSkinningTypeDiscrimination(PSOType type)
{
	SkinningCommon* skinning = entityManager->GetSkinningCommon();

	// 描画前準備
	skinning->DrawComputeSetting();

	// メッシュごとに
	for (auto& mesh : model->GetModelData().mesh) {

		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(1, mesh->skinCluster->paletteSrvHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(2, mesh->skinCluster->inputVertexSrvHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(3, mesh->skinCluster->influenceSrvHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(4, mesh->skinCluster->outputVertexUavHandle.second);
		skinning->GetDxCommon()->GetCommandList()->SetComputeRootConstantBufferView(0, mesh->skinCluster->skinningInformation->GetGPUVirtualAddress());


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

void Engine::RenderComponent::DrawSetting()
{
	ShadowMap* shadowMap = entityManager->GetObject3dCommon()->GetDxCommon()->GetShadowMap();

	// ライト
	entityManager->GetLightManager()->DrawLight();

	// 位置
	transformation->GetCommandList(1);
	transformation->GetCommandList(10);

	// カメラ
	camera->GetCommandList(4);

	// 通常描画でライト視点の深度を参照し、平行光源の影を計算する。
	shadowMap->SetGraphicsRootDescriptorTable(11);
	shadowMap->SetGraphicsRootConstantBufferView(12);
}

void Engine::RenderComponent::DrawSettingSkin()
{
	ShadowMap* shadowMap = entityManager->GetObject3dCommon()->GetDxCommon()->GetShadowMap();

	// ライト
	entityManager->GetLightManager()->DrawLight();
	// 位置
	transformation->GetCommandList(1);
	// カメラ
	camera->GetCommandList(4);
	// スキニングモデルも通常モデルと同じシャドウマップを参照する。
	shadowMap->SetGraphicsRootDescriptorTable(10);
	shadowMap->SetGraphicsRootConstantBufferView(11);
}

void Engine::RenderComponent::DrawSettingOcean()
{
	// 描画前準備
	entityManager->GetOceanManager()->DrawCommonSetting();

	// ライト
	entityManager->GetLightManager()->DrawLight();
	// 位置
	transformation->GetCommandList(1);
	transformation->GetCommandList(9);
	// カメラ
	camera->GetCommandList(4);
}

#pragma region MyRegion

void Engine::RenderComponent::SetModel(Engine::Model* model) {
	this->model = model;

	materialInstances_.clear();
	int i = 0;
	for (auto& mesh : model->GetModelData().mesh) {
		MaterialInstance matInst;

		std::unique_ptr<ConstantBuffer<Material::DataGPU>> cb =
			std::make_unique<ConstantBuffer<Material::DataGPU>>();
		cb->CreateBuffer(entityManager->GetCameraCommon()->GetDxCommon());
		cbResources_.push_back(std::move(cb));


		matInst = mesh->material->GetMaterialInstance();
		materialInstances_.push_back(matInst);
	}
}

// スカイボックス設定
void Engine::RenderComponent::SetSkyBox(Engine::SkyBox* skyBox) { this->skyBox = skyBox; }
// 波セット設定
void Engine::RenderComponent::SetOcean(Engine::Ocean* ocean) { this->ocean = ocean; }
// プリミティブ設定
void Engine::RenderComponent::SetPrimitive(std::unique_ptr<Engine::BasePrimitive> primitive) { this->primitive = std::move(primitive); }

// モデル取得
Engine::Model* Engine::RenderComponent::GetModel() const { return model; }
// 波取得
Engine::Ocean* Engine::RenderComponent::GetOcean() { return ocean; }
// スカイボックス取得
Engine::SkyBox* Engine::RenderComponent::GetSkyBox() { return skyBox; }
// プリミティブ形状取得
Engine::BasePrimitive* Engine::RenderComponent::GetPrimitive() { return primitive.get(); }

#pragma endregion


