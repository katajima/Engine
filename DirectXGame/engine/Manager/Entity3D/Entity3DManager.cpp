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
	effectManager_->Initialize(directXCommon_,lightManager_.get());
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
		ImGui::DragFloat3("scale", &entity->worldtransform_.scale_.x, 0.1f);
		ImGui::DragFloat3("rotate", &entity->worldtransform_.rotate_.x, 0.1f);
		ImGui::DragFloat3("translate", &entity->worldtransform_.translate_.x, 0.1f);
	}

	ImGui::End();

#endif // _DEBUG

}

void Entity3DManager::ObjectClean()
{
	object3d.clear();
}

void Entity3DManager::SetEntity3D(Object3d* entity3D)
{
	object3d.push_back(std::move(entity3D));
}


