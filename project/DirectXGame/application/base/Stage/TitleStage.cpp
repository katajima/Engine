#include "TitleStage.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

#include "DirectXGame/engine/MyGame/MyGame.h"

void TitleStage::Initialize(Engine::DirectXCommon* dxcommon, Engine::Entity3DManager* entity3DManager, 
	Engine::Entity2DManager* entity2DManager, Engine::Camera* camera) {
	this->dxCommon = dxcommon;				// ダイレクトX共通クラス
	this->entity3DManager = entity3DManager;	// エンティティ3d
	this->entity2DManager = entity2DManager;	// エンティティ2d

	// スカイボックス初期化
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(entity3DManager, "resources/Texture/hdr/sky.dds");

	// 空
	sky_ = entity3DManager->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {}, camera);
	sky_->GetWorldTransform().scale_ = provisionalData_.skyBoxScale;
	sky_->SetSkyBox(skyBox.get());

	// タイル
	tail = entity3DManager->CreateObject3D("tail", Engine::ObjectModelType::kNormal, {}, camera);
	tail->SetModel("Ground.obj");
	tail->GetWorldTransform().scale_ = { 100,100,100 };
	tail->GetMaterial(0)->GetMaterialInstance().transform.scale = { 100,100,100 };

	// ライト
	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 1.5f;
	directionalLightData.isLight = true;
	directionalLightData.lig = 0.1f;
	directional = std::make_shared<Engine::DirectionalLight>();
	directional->directional = directionalLightData;
	entity3DManager->GetLightManager()->AddLight(directional);
};

void TitleStage::Update() {

};

void TitleStage::Draw() {
};

void TitleStage::DrawP() {
};

void TitleStage::Draw2D() {
};


void TitleStage::InitEmit() {
};

void TitleStage::EmitUpdate() {

};
