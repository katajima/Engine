#include "TitleStage.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Effect/Effect.h"


void TitleStage::Initialize(Engine::DirectXCommon* dxcommon, Engine::Entity3DManager* entity3DManager, 
	Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Engine::Camera* camera) {
	this->dxCommon = dxcommon;				// ダイレクトX共通クラス
	this->entity3DManager = entity3DManager;	// エンティティ3d
	this->entity2DManager = entity2DManager;	// エンティティ2d
	this->globalVariables = globalVariables;	// 保存項目
	this->camera = camera;						// カメラ

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

	// 車
	playerCar_ = std::make_unique<PlayerCar>();
	playerCar_->Initialize(entity3DManager, globalVariables,playerCarPos_, { 0,Math::DegreesToRadians(-90),0 });
	playerCar_->SetGroungHeight(0.1f);
	

	// 石初期化
	InitializeStone();

	// ライト
	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 0.9f;
	directionalLightData.isLight = true;
	directionalLightData.lig = 0.1f;
	directional = std::make_shared<Engine::DirectionalLight>();
	directional->directional = directionalLightData;
	entity3DManager->GetLightManager()->AddLight(directional);

	PointLightData pointLightData{};
	pointLightData.color = { 1,1,1,1 };
	pointLightData.position = { 0,100,0 };
	pointLightData.intensity = 1.0f;
	pointLightData.radius = 100000;
	pointLightData.isLight = true;
	//
	pointLight = std::make_shared<Engine::PointLight>();
	pointLight->point = pointLightData;
};

void TitleStage::InitializeStone(){
	stone1_ = entity3DManager->CreateObject3D("Stone1", Engine::ObjectModelType::kNormal, {}, camera);
	stone1_->SetModel("Stone.obj");
	stone1_->GetWorldTransform().scale_ = { 1,1,1 };
	stone1_->GetWorldTransform().translate_ = { -260,-20,700 };

	stone2_ = entity3DManager->CreateObject3D("Stone2", Engine::ObjectModelType::kNormal, {}, camera);
	stone2_->SetModel("Stone.obj");
	stone2_->GetWorldTransform().scale_ = { 1,1,1 };
	stone2_->GetWorldTransform().translate_ = { 260,-20,500 };

	stone3_ = entity3DManager->CreateObject3D("Stone3", Engine::ObjectModelType::kNormal, {}, camera);
	stone3_->SetModel("Stone.obj");
	stone3_->GetWorldTransform().scale_ = { 1,1,1 };
	stone3_->GetWorldTransform().translate_ = { -160,-25,1000 };

	stone4_ = entity3DManager->CreateObject3D("Stone4", Engine::ObjectModelType::kNormal, {}, camera);
	stone4_->SetModel("Stone.obj");
	stone4_->GetWorldTransform().scale_ = { 1,1,1 };
	stone4_->GetWorldTransform().translate_ = { 200,-28,900 };

	stone5_ = entity3DManager->CreateObject3D("Stone5", Engine::ObjectModelType::kNormal, {}, camera);
	stone5_->SetModel("Stone.obj");
	stone5_->GetWorldTransform().scale_ = { 1,1,1 };
	stone5_->GetWorldTransform().translate_ = { 0,-28,2000 };


};

void TitleStage::Update(float dt) {

	// エフェクト
	effect->Emit("dust3", {-50,10,100});
	effect->Emit("dust3", { -50,10,100 });


	playerCar_->Update(dt);
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

}

