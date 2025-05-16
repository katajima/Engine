#include "Entity3DManager.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"


void Entity3DManager::Initialize(DirectXCommon* directXCommon)
{
	directXCommon_ = directXCommon;

	// ライト
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize(directXCommon_);

	cameraCommon_ = std::make_unique<CameraCommon>();
	cameraCommon_->Initialize(directXCommon_);

	// オーシャンシェーダー
	oceanManager_ = std::make_unique<OceanManager>();
	oceanManager_->Initialize(directXCommon_);


	object3dInstansManager_ = std::make_unique<Object3dInstansManager>();
	object3dInstansManager_->Initialize(directXCommon_);


	object3dCommon_ = std::make_unique<Object3dCommon>();
	object3dCommon_->Initialize(directXCommon_);

	// スカイボックス
	skyBoxCommon_ = std::make_unique<SkyBoxCommon>();
	skyBoxCommon_->Initialize(directXCommon_);

	

	// スキニング
	skinningCommon_ = std::make_unique<SkinningConmmon>();
	skinningCommon_->Initialize(directXCommon_);

	lineCommon_ = std::make_unique<LineCommon>();
	lineCommon_->Initialize(directXCommon_);

	// プリミティブ
	primitiveCommon_ = std::make_unique<PrimitiveCommon>();
	primitiveCommon_->Initialize(directXCommon_);


	// エフェクトマネージャー
	effectManager_ = std::make_unique<EffectManager>();
	effectManager_->Initialize(directXCommon_,lightManager_.get(),lineCommon_.get());
}

void Entity3DManager::UpdateImgui()
{
#ifdef _DEBUG

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
		std::string objectTypeName = "ObjectType : " + entity->GetObjectTypeName();
		ImGui::Text(objectTypeName.c_str());
		ImGui::Separator();

		Material* material;
		BaseMesh* mesh;
		
		if (entity->GetObjectType() == Object3d::ObjectType::kSkyBox) {
			material = entity->skyBox_->GetMaterial();
			mesh = entity->skyBox_->GetMesh();
		}
		else if (entity->GetObjectType() == Object3d::ObjectType::kPrimitive) {
			material = entity->primitive_->GetMaterial();
			mesh = entity->primitive_->GetMesh();
		}else if (entity->GetObjectType() == Object3d::ObjectType::kOcean) {
			material = entity->ocean_->GetMaterial();
			mesh = entity->ocean_->GetMesh();
		}else{
			if (entity->GetObjectType() == Object3d::ObjectType::kNormal) {
				material = entity->GetMaterial(0);
				mesh = entity->GetMesh(0);
			}
			else if (entity->GetObjectType() == Object3d::ObjectType::kAnimation) {
				material = entity->GetMaterial(0);
				mesh = entity->GetMesh(0);
			}
			else if (entity->GetObjectType() == Object3d::ObjectType::kSkinning) {
				material = entity->GetMaterial(0);
				mesh = entity->GetMesh(0);
			}
		}

		ImGui::Separator();
		ImGui::Text("transform");
		ImGui::Separator();
		ImGui::DragFloat3("T_scale", &entity->worldtransform_.scale_.x, 0.1f);
		ImGui::DragFloat3("T_rotate", &entity->worldtransform_.rotate_.x, 0.1f);
		ImGui::DragFloat3("T_translate", &entity->worldtransform_.translate_.x, 0.1f);
		ImGui::Separator();
		ImGui::Text("material");
		ImGui::Separator();
		ImGui::DragFloat3("M_scale", &material->transform.scale.x, 0.1f);
		ImGui::DragFloat3("M_rotate", &material->transform.rotate.x, 0.1f);
		ImGui::DragFloat3("M_translate", &material->transform.translate.x, 0.1f);
		ImGui::ColorEdit4("color", &material->color.r);
		ImGui::SliderInt("enableLighting", &material->enableLighting_, 0,1);
		ImGui::SliderFloat("alphaClipping", &material->alphaClipping_, 0,1);
		ImGui::DragFloat("shininess", &material->shininess_,0.01f);
		ImGui::Separator();
		ImGui::Text("mesh");
		ImGui::Separator();

		// メッシュ

		

		//int index = static_cast<int>(mesh->vertices.size());
		//ImGui::InputInt("verticesSize", &index);
		//index = static_cast<int>(mesh->indices.size());
		//ImGui::InputInt("indicesSize", &index);
		//index = static_cast<int>(mesh->indices.size());

		
		
		

		// プリミティブ形状なら
		if (entity->GetPrimitive()) {
			ImGui::Separator();
			entity->GetPrimitive()->MeshUpdateImGui();
		}

		if (entity->GetOcean()) {
			ImGui::Separator();
			entity->GetOcean()->UpdateImgui();
		}

		if (entity->GetObjectType() == Object3d::ObjectType::kSkinning) {
			entity->DebugImguiSkin();
		}

	}

	ImGui::End();

#endif // _DEBUG

}

void Entity3DManager::Update()
{

	object3d.erase(
		std::remove_if(object3d.begin(), object3d.end(),
			[](Object3d* object) {
				return object->GetIsDelete();
			}),
		object3d.end());


	for (auto& object : object3d) {
		if (object != nullptr) {
			object->Update();
		}
	}
}

void Entity3DManager::ObjectClean()
{
	object3d.clear();
}

void Entity3DManager::SetEntity3D(Object3d* entity3D)
{
	object3d.push_back(std::move(entity3D));
}


