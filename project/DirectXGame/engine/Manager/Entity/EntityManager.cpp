#include "EntityManager.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

using namespace Engine;

std::mutex mutex;  // グローバルやスコープ内に必要

void Engine::EntityManager::Initialize(DirectXCommon* directXCommon)
{
	dxCommon = directXCommon;	// DX共通クラス

	// ライト共通クラス初期化
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize(dxCommon);

	// カメラ共通クラス初期化
	cameraCommon_ = std::make_unique<CameraCommon>();
	cameraCommon_->Initialize(dxCommon);

	// オーシャンシェーダー初期化
	oceanManager_ = std::make_unique<OceanManager>();
	oceanManager_->Initialize(dxCommon);

	// オブジェクトのインスタンシング初期化
	object3dInstansManager_ = std::make_unique<Object3dInstansManager>();
	object3dInstansManager_->Initialize(dxCommon);
	object3dInstansManager_->SetEntity3D(this);

	// オブジェクト共通クラス初期化
	object3dCommon_ = std::make_unique<Object3dCommon>();
	object3dCommon_->Initialize(dxCommon);

	// スカイボックス共通クラス初期化
	skyBoxCommon_ = std::make_unique<SkyBoxCommon>();
	skyBoxCommon_->Initialize(dxCommon);



	// スキニング共通クラス初期化
	skinningCommon_ = std::make_unique<SkinningConmmon>();
	skinningCommon_->Initialize(dxCommon);

	// ライン共通クラス初期化
	lineCommon_ = std::make_unique<LineCommon>();
	lineCommon_->Initialize(dxCommon);

	// プリミティブ共通クラス初期化
	primitiveCommon_ = std::make_unique<PrimitiveCommon>();
	primitiveCommon_->Initialize(dxCommon);


	// エフェクトマネージャー初期化
	effectManager_ = std::make_unique<EffectManager>();
	effectManager_->Initialize(dxCommon, lightManager_.get(), lineCommon_.get());

	// スプライト共通クラス初期化
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxCommon);

}

void Engine::EntityManager::UpdateImgui()
{
#ifdef _DEBUG
	ImGui::Begin("EmitParticle");
	ImGui::End();
	std::string title = reinterpret_cast<const char*>(ICON_MD_HOME);

	title += " SceneCollection";
	ImGui::Begin(title.c_str());
	static int selectedIndex_ = -1;

	int openedIndex = -1;
	for (size_t i = 0; i < object3d.size(); ++i) {
		auto& entity = object3d[i];

		// 強制的に一つだけ開くように設定
		ImGui::SetNextItemOpen(i == selectedIndex_, ImGuiCond_Always);
		if (ImGui::TreeNode(object3d[i]->name.c_str())) {
			openedIndex = static_cast<int>(i);  // 開いているインデックスを記録
			selectedIndex_ = openedIndex;
			ImGui::TreePop();
		}
	}
	ImGui::End();

	ImGui::Begin("Object Properties");
	if (openedIndex >= 0 && openedIndex < static_cast<int>(object3d.size())) {
		auto& entity = object3d[openedIndex];
		ImGui::Text(entity->name.c_str());
		ImGui::Separator();
		std::string objectTypeName = "ObjectType : " + entity->GetRenderComponent()->GetObjectTypeName();
		ImGui::Text(objectTypeName.c_str());
		ImGui::Separator();

		ImGui::Text("transform");
		ImGui::Separator();
		ImGui::DragFloat3("T_scale", &entity->GetWorldTransform().scale_.x, 0.1f);
		ImGui::DragFloat3("T_rotate", &entity->GetWorldTransform().rotate_.x, 0.1f);
		ImGui::DragFloat3("T_translate", &entity->GetWorldTransform().translate_.x, 0.1f);

		std::string nameColliderComponent = "";
		if (entity->GetColliderComponent()) {
			if (ImGui::CollapsingHeader("ColliderComponent")) {
				int collIndex = 0;
				for (auto& coll : entity->GetColliderComponent()->GetAllColliders()) {
					nameColliderComponent = "Collider" + std::to_string(collIndex);
					if (ImGui::CollapsingHeader(nameColliderComponent.c_str())) {
						ImGui::InputFloat3("position", &coll->centerWorld.x);
					}
					collIndex++;
				}
			}
		}

		if (entity->GetRenderComponent()->GetIsSkin()) {
			ImGui::Separator();
			ImGui::Text("material");
			ImGui::Separator();

			Material* material;
			int materialIndex = 0;
			std::string nameMaterial = "";
			std::string nameMesh = "";

			if (entity->GetRenderComponent()->GetObjectType() == ObjectModelType::kSkyBox) {
				material = entity->GetSkyBox()->GetMaterial();
				nameMaterial = "Material" + std::to_string(materialIndex);
				if (ImGui::CollapsingHeader(nameMaterial.c_str())) {
					ImGui::DragFloat3("M_scale", &material->GetMaterialInstance().transform.scale.x, 0.1f);
					ImGui::DragFloat3("M_rotate", &material->GetMaterialInstance().transform.rotate.x, 0.1f);
					ImGui::DragFloat3("M_translate", &material->GetMaterialInstance().transform.translate.x, 0.1f);
					ImGui::ColorEdit4("color", &material->GetMaterialInstance().color.r);
					ImGui::SliderInt("enableLighting", &material->GetMaterialInstance().enableLighting_, 0, 1);
					ImGui::SliderFloat("alphaClipping", &material->GetMaterialInstance().alphaClipping_, 0, 1);
					ImGui::DragFloat("shininess", &material->GetMaterialInstance().shininess_, 0.01f);
				}

			}
			else if (entity->GetRenderComponent()->GetObjectType() == ObjectModelType::kPrimitive) {
			}
			else if (entity->GetRenderComponent()->GetObjectType() == ObjectModelType::kOcean) {
				material = entity->GetOcean()->GetMaterial();
				nameMaterial = "Material" + std::to_string(materialIndex);
				if (ImGui::CollapsingHeader(nameMaterial.c_str())) {
					ImGui::DragFloat3("M_scale", &material->GetMaterialInstance().transform.scale.x, 0.1f);
					ImGui::DragFloat3("M_rotate", &material->GetMaterialInstance().transform.rotate.x, 0.1f);
					ImGui::DragFloat3("M_translate", &material->GetMaterialInstance().transform.translate.x, 0.1f);
					ImGui::ColorEdit4("color", &material->GetMaterialInstance().color.r);
					ImGui::DragFloat("alpha", &material->GetMaterialInstance().alpha_);
					ImGui::SliderInt("enableLighting", &material->GetMaterialInstance().enableLighting_, 0, 1);
					ImGui::SliderFloat("alphaClipping", &material->GetMaterialInstance().alphaClipping_, 0, 1);
					ImGui::DragFloat("shininess", &material->GetMaterialInstance().shininess_, 0.01f);
				}

			}
			else {
				for (auto& mesh : entity->GetModel()->GetModelData().mesh) {
					nameMesh = "Mesh" + mesh->name;
					if (ImGui::CollapsingHeader(nameMesh.c_str())) {
						int verticesCount = static_cast<int>(mesh->vertices.size());
						int indexCount = static_cast<int>(mesh->GetIndices().size());
						ImGui::InputInt("vertices", &verticesCount);
						ImGui::InputInt("index", &indexCount);
						ImGui::InputFloat3("position", &mesh->position.x);
					}
				}
				ImGui::Separator();


				for (auto& material : entity->GetMaterialInstance()) {

					nameMaterial = "Material" + std::to_string(materialIndex);
					if (ImGui::CollapsingHeader(nameMaterial.c_str())) {
						ImGui::DragFloat3("M_scale", &material.transform.scale.x, 0.1f);
						ImGui::DragFloat3("M_rotate", &material.transform.rotate.x, 0.1f);
						ImGui::DragFloat3("M_translate", &material.transform.translate.x, 0.1f);
						ImGui::ColorEdit4("color", &material.color.r);
						ImGui::SliderInt("enableLighting", &material.enableLighting_, 0, 1);
						ImGui::SliderFloat("alphaClipping", &material.alphaClipping_, 0, 1);
						ImGui::DragFloat("shininess", &material.shininess_, 0.01f);
						ImGui::SliderFloat("alpha", &material.alpha_, 0, 1);
					}
					materialIndex++;
				}

				if (ImGui::CollapsingHeader("Animetion")) {
					for (auto& anima : entity->GetModel()->GetModelData().animations) {
						ImGui::Text(anima.first.c_str());
					}
				}
			}

			// プリミティブ形状なら
			if (entity->GetPrimitive()) {
				ImGui::Separator();
			}



			if (entity->GetOcean()) {
				ImGui::Separator();
				entity->GetOcean()->UpdateImgui();
			}

			if (entity->GetRenderComponent()->GetObjectType() == ObjectModelType::kSkinning) {
				entity->DebugImguiSkin();
			}
		}


	}

	ImGui::End();

#endif // _DEBUG

}

void Engine::EntityManager::Update()
{
	// オブジェクトインスタンシング更新
	object3dInstansManager_->Update();

	// オブジェクトが消えたときの処理
	object3d.erase(
		std::remove_if(object3d.begin(), object3d.end(),
			[](const std::unique_ptr<Object3d>& object) {
				return object->GetIsDelete();
			}),
		object3d.end());


	// オブジェクトの更新
	for (auto& object : object3d) {
		if (object) {
			// 物理更新
			object->RigidBodyUpdate();
		}
	}

	for (auto& object : object3d) {
		if (object) {
			// 更新
			object->Update();
			// オブジェクトの描画順決定
			switch (object->GetRenderComponent()->GetObjectDrawType()) {
			case ObjectDrawType::kTranslucent01:
				transparentObjects01.push_back(object.get());
				break;
			case ObjectDrawType::kTranslucent02:
				transparentObjects02.push_back(object.get());
				break;
			case ObjectDrawType::kTranslucent03:
				transparentObjects03.push_back(object.get());
				break;
			case ObjectDrawType::kOpaque:
				if (object->GetAlpha() < 1.0f) {
					transparentObjects01.push_back(object.get());
				}
				else {
					opaqueObjects.push_back(object.get());
				}
				break;
			}
		}
	}
}

void Engine::EntityManager::ObjectClean()
{
	object3d.clear();
}

void Engine::EntityManager::ObjectDraw()
{
	// インスタンシング描画
	object3dInstansManager_->Draw();

	// 不透明
	for (auto& object : opaqueObjects) {
		object->Draw();
	}
	opaqueObjects.clear();


	object3dInstansManager_->DrawTransparency();

	// 半透明最初
	for (auto& object : transparentObjects01) {
		object->Draw();
	}
	transparentObjects01.clear();

	// 半透明中盤
	for (auto& object : transparentObjects02) {
		object->Draw();
	}
	transparentObjects02.clear();

	// 半透明最後
	for (auto& object : transparentObjects03) {
		object->Draw();
	}
	transparentObjects03.clear();


	// トレイルエフェクト
	for (auto& object : object3d) {
		object->DrawTrailEffect();
	}

}

void Engine::EntityManager::DrawShadowMap(ShadowMap* shadowMap)
{
	object3dInstansManager_->DrawShadowMap(shadowMap);

	for (auto& object : object3d) {
		if (object) {
			object->DrawShadowMap(shadowMap);
		}
	}
}



