#include"Object3dCommon.h"
#include"Object3d.h"
#include"DirectXGame/engine/Skinning/Skinning.h"
#include<cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include<dxcapi.h>
#include<fstream>
#include<sstream>
#include <iostream>
#include<wrl.h>
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/MyGame/MyGame.h"



#include"DirectXGame/engine/Animation/Animation.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"

void Object3d::Initialize(Entity3DManager* entity3DManager, ObjectType objectType, ObjectRasterizerType rasterizerType)
{
	entity3DManager_ = entity3DManager;
	object3dCommon_ = entity3DManager_->GetObject3dCommon();
	skinningConmmon_ = entity3DManager_->GetSkinningConmmon();
	imGuiManager_ = entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetImGuiManager();
	skyBoxCommon_ = entity3DManager_->GetSkyBoxCommon();
	oceanManager_ = entity3DManager_->GetOceanManager();

	worldtransform_.Initialize();
	worldtransform_.translate_.x = { 0.00000001f };

	name = "object" + std::to_string(object3dCommon_->count);

	transfomation = std::make_unique<Transfomation>();
	transfomation->Initialize(object3dCommon_->GetDxCommon());


	entity3DManager_->SetEntity3D(this);

	// オブジェクトタイプ
	objectType_ = objectType;

	switch (objectType)
	{
	case Object3d::ObjectType::kNormal:
		objectTypeName = "NormalModelObject";
		break;
	case Object3d::ObjectType::kAnimation:
		objectTypeName = "AnimationModelObject";
		break;
	case Object3d::ObjectType::kSkinning:
		objectTypeName = "SkinningModelObject";
		break;
	case Object3d::ObjectType::kPrimitive:
		objectTypeName = "PrimitiveObject";
		break;
	case Object3d::ObjectType::kSkyBox:
		objectTypeName = "SkyBoxObject";
		break;
	case Object3d::ObjectType::kOcean:
		objectTypeName = "OceanObject";
		break;
	default:
		objectTypeName = "NoObject";
		break;
	}

	// 映り方
	rasterizerType_ = rasterizerType;

	// オブジェクト数
	object3dCommon_->count++;
}

#pragma region Update

void Object3d::Update()
{
	Matrix4x4 localMatrix = MakeIdentity4x4();

	worldtransform_.Update();

	switch (objectType_)
	{
	case Object3d::ObjectType::kNormal:
		// モデルが存在する場合
		if (model) {
			localMatrix = model->modelData.rootNode.localMatrix;
			model->modelData.material[0]->GPUData();
		}

		// トランスフォームデータ
		transfomation->Update(model, camera, localMatrix, worldtransform_.worldMat_);
		break;
	case Object3d::ObjectType::kAnimation:
		// モデルが存在する場合
		if (model) {
			// アニメーションの更新
			if (model->modelData.animation.flag) {
				if (flag) {
					model->modelData.animationTime += MyGame::GameTime(); // フレームごとの時間経過を反映
				}
				model->modelData.animationTime = std::fmod(model->modelData.animationTime, model->modelData.animation.duration);

				// 単一のジョイントの場合
				const NodeAnimation& rootNodeAnimation = model->modelData.animation.nodeAnimations[model->modelData.rootNode.name];
				Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, model->modelData.animationTime);
				Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, model->modelData.animationTime);
				Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, model->modelData.animationTime);
				localMatrix = MakeAffineMatrix(scale, rotate, translate);
			}
			else {
				localMatrix = model->modelData.rootNode.localMatrix;
			}
			model->modelData.material[0]->GPUData();
		}

		// トランスフォームデータ
		transfomation->Update(model, camera, localMatrix, worldtransform_.worldMat_);
		break;
	case Object3d::ObjectType::kSkinning:

		// モデルが存在する場合
		if (model) {
			// アニメーションの更新
			if (model->modelData.animation.flag) {
				if (flag) {
					model->modelData.animationTime += MyGame::GameTime(); // フレームごとの時間経過を反映
				}
				model->modelData.animationTime = std::fmod(model->modelData.animationTime, model->modelData.animation.duration);
				localMatrix = model->modelData.skeleton.joints[0].skeletonSpaceMatrix;

				ApplyAnimation(model->modelData.skeleton, model->modelData.animation, model->modelData.animationTime);
				// スケルトンの更新
				UpdateSkeleton(model->modelData.skeleton);

				// スキニング更新
				UpdateSkinCluster(model->modelData.skinCluster, model->modelData.skeleton);

				DrawSkeleton(entity3DManager_->Get3DLineCommon(), model->modelData.skeleton.joints, worldtransform_.worldMat_.GetWorldPosition(), worldtransform_.scale_);
			}
			else {
				localMatrix = model->modelData.rootNode.localMatrix;
			}
			model->modelData.material[0]->GPUData();
		}

		// トランスフォームデータ
		transfomation->UpdateSkinning(model, camera, localMatrix, worldtransform_.worldMat_);
		break;
	case ObjectType::kPrimitive:
		if (primitive_) {
			primitive_->Update();

			transfomation->Update(primitive_, camera, localMatrix, worldtransform_.worldMat_);
		}
		break;
	case ObjectType::kSkyBox:
		if (skyBox_) {
			skyBox_->Update();

			transfomation->Update(primitive_, camera, localMatrix, worldtransform_.worldMat_);
		}
		break;
	case ObjectType::kOcean:
		if (ocean_) {
			ocean_->Update();

			transfomation->Update(ocean_, camera, localMatrix, worldtransform_.worldMat_);
		}
		break;
	default:
		break;
	}
}
#pragma endregion //更新系

#pragma region Draw

void Object3d::Draw()
{
	switch (objectType_)
	{
	case Object3d::ObjectType::kNormal:
		ObjectTypeDiscrimination(rasterizerType_);

		DrawSetting();


		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->Draw();
		}
		break;
	case Object3d::ObjectType::kAnimation:
		ObjectTypeDiscrimination(rasterizerType_);

		DrawSetting();


		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->Draw();
		}
		break;
	case Object3d::ObjectType::kSkinning:
		ObjectSkinTypeDiscrimination(rasterizerType_);

		DrawSettingSkin();

		// 3Dモデルが割り当てれていれば描画する
		if (model) {
			model->DrawSkinning();
		}
		break;
	case ObjectType::kPrimitive:


		if (primitive_) {

			primitive_->DrawSetting();

			transfomation->GetCommandList(1);

			primitive_->Draw();
		}
		break;

	case ObjectType::kSkyBox:

		if (skyBox_) {
			skyBoxCommon_->DrawCommonSetting();

			transfomation->GetCommandList(1);

			skyBox_->Draw();
		}
		break;
	case ObjectType::kOcean:

		if (ocean_) {
			oceanManager_->DrawCommonSetting();

			DrawSettingOcean();

			ocean_->Draw();
		}
		break;
	}




}


Vector2 Object3d::GetScreenPosition()
{
	Vector3 wPos = worldtransform_.worldMat_.GetWorldPosition();

	// カメラのビュープロジェクション行列を取得
	Matrix4x4 matViewProjection = Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix());

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, 1280, 720, 0, 1);

	// 視錐台内にオブジェクトがあるかチェック (matViewProjection を渡す)
	if (!IsInFrustum(matViewProjection, wPos)) {
		return Vector2{ -100, -100 }; // 視錐台外にある場合、無効なスクリーン座標を返す
	}

	// ワールド座標をクリップ空間座標へ変換
	Vector4 clipSpacePos = Transforms(Vector4(wPos.x, wPos.y, wPos.z, 1.0f), matViewProjection);

	// 透視除算 (NDC へ変換)
	if (clipSpacePos.w == 0.0f) {
		return Vector2{ -100, -100 }; // 透視除算エラー
	}
	Vector3 ndcPos = {
		clipSpacePos.x / clipSpacePos.w,
		clipSpacePos.y / clipSpacePos.w,
		clipSpacePos.z / clipSpacePos.w
	};

	// NDC → スクリーン座標変換
	Vector4 screenPos = Transforms(Vector4(ndcPos.x, ndcPos.y, ndcPos.z, 1.0f), matViewport);

	return Vector2{ screenPos.x, screenPos.y };
}


bool Object3d::IsInFrustum(const Matrix4x4& viewProjectionMatrix, const Vector3& position)
{
	// クリップスペース座標を取得
	Vector4 clipSpacePosition = Transforms(Vector4(position.x, position.y, position.z, 1.0f), viewProjectionMatrix);

	// w が負の場合、カメラの後ろにあるため視錐台外
	if (clipSpacePosition.w <= 0.0f) {
		return false;
	}

	// 視錐台内にあるかチェック
	if (clipSpacePosition.x < -clipSpacePosition.w || clipSpacePosition.x > clipSpacePosition.w ||
		clipSpacePosition.y < -clipSpacePosition.w || clipSpacePosition.y > clipSpacePosition.w ||
		clipSpacePosition.z < 0 || clipSpacePosition.z > clipSpacePosition.w)
	{
		return false;
	}

	return true;
}


void Object3d::DebugImguiModel()
{
	if (ImGui::CollapsingHeader("Modeldata")) {
		if (ImGui::TreeNode("rootNode")) {
			for (size_t i = 0; i < model->modelData.rootNode.children.size(); ++i) {
				std::string name = "children" + std::to_string(i);
				if (ImGui::TreeNode(name.c_str())) {
					ImGui::DragFloat3("translate", &model->modelData.rootNode.children[i].transform.translate.x);
					ImGui::DragFloat3("rotate", &model->modelData.rootNode.children[i].transform.rotate.x);
					ImGui::DragFloat3("scale", &model->modelData.rootNode.children[i].transform.scale.x);

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}

void Object3d::DebugImguiSkin()
{

	
	if (ImGui::CollapsingHeader("SkinnigData")) {
		int index = static_cast<int>(model->modelData.skinning.influencesIndex);
		ImGui::InputInt("influencesIndex", &index);
		index = static_cast<int>(model->modelData.skinning.wellSrvIndex);
		ImGui::InputInt("wellSrvIndex", &index);
		index = static_cast<int>(model->modelData.skinning.inputVerticesIndex);
		ImGui::InputInt("inputVerticesIndex", &index);
		index = static_cast<int>(model->modelData.skinning.outputVerticesUavIndex);
		ImGui::InputInt("outputVerticesUavIndex", &index);
		index = static_cast<int>(model->modelData.skinCluster.skinningInfomationDeta->numVertices);
		ImGui::InputInt("numVertices", &index);

		

	}
}

void Object3d::DrawSetting()
{
	entity3DManager_->GetLightManager()->DrawLight();

	transfomation->GetCommandList(1);



	transfomation->GetCommandList(10);



	camera->GetCommandList(4);
}

void Object3d::DrawSettingSkin()
{

	entity3DManager_->GetLightManager()->DrawLight();

	transfomation->GetCommandList(1);

	camera->GetCommandList(4);
}

void Object3d::DrawSettingOcean()
{
	entity3DManager_->GetOceanManager()->DrawCommonSetting();


	entity3DManager_->GetLightManager()->DrawLight();

	transfomation->GetCommandList(1);
	transfomation->GetCommandList(9);

	camera->GetCommandList(4);
}

void Object3d::ObjectTypeDiscrimination(ObjectRasterizerType type)
{
	switch (type)
	{
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		object3dCommon_->DrawCommonSetting(Object3dCommon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE);
		break;
	default:
		break;
	}


}

void Object3d::ObjectSkinTypeDiscrimination(ObjectRasterizerType type)
{
	skinningConmmon_->DrawComputeSetting();


	skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(1, model->modelData.skinCluster.paletteSrvHandle.second);
	skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(2, model->modelData.skinCluster.inputVertexSrvHandle.second);
	skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(3, model->modelData.skinCluster.influenceSrvHandle.second);
	skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootDescriptorTable(4, model->modelData.skinCluster.outputVertexUavHandle.second);
	skinningConmmon_->GetDxCommon()->GetCommandList()->SetComputeRootConstantBufferView(0, model->modelData.skinCluster.skinningInfomation->GetGPUVirtualAddress());


	skinningConmmon_->GetDxCommon()->GetCommandList()->Dispatch(UINT(model->modelData.mesh[0]->vertices.size() + 1023) / 1024, 1, 1);

	// 初期状態を UAV 用に遷移させる
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = model->modelData.skinCluster.outputVertexResource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	skinningConmmon_->GetDxCommon()->GetCommandList()->ResourceBarrier(1, &barrier);


	switch (type)
	{
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_SOLID_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_NONE);
		break;
	case Object3d::ObjectRasterizerType::UvInterpolation_MODE_WIREFRAME_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE);
		break;
	case Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		skinningConmmon_->DrawCommonSetting(SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE);
		break;
	default:
		break;
	}
	

}

#pragma endregion // 描画系

void Object3d::SetModel(const std::string& filePath)
{
	//モデルを検索してセット

	model = object3dCommon_->GetDxCommon()->GetModelManager()->FindModel(filePath);
}
