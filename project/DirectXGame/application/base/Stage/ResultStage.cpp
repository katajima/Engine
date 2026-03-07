#include "ResultStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/engine/Math/Random.h"

void ResultStage::Initialize(Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Engine::Camera* camera) {
	this->entityManager = entityManager;	// エンティティ3d
	this->globalVariables = globalVariables;	// 保存項目
	this->camera = camera;						// カメラ


	// スカイボックス初期化
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(entityManager, "resources/Texture/hdr/sky.dds");

	// 空
	sky_ = entityManager->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {}, camera);
	sky_->GetWorldTransform().scale_ = { 100,100,100 };
	sky_->SetSkyBox(skyBox.get());

	// タイル
	tail = entityManager->CreateObject3D("tail", Engine::ObjectModelType::kNormal, {}, camera);
	tail->SetModel("Ground.obj");
	tail->GetWorldTransform().scale_ = { 100,100,100 };
	tail->GetMaterial(0)->GetMaterialInstance().transform.scale = { 100,100,100 };

	// 換金所
	moneyExchangePlace = entityManager->CreateObject3D("MoneyExchangePlace", Engine::ObjectModelType::kNormal, {0,0.06f,0}, camera);
	moneyExchangePlace->SetModel("MoneyExchangePlace.obj");
	moneyExchangePlace->GetWorldTransform().rotate_ = { 0,Math::DegreesToRadians(-90),0 };

	// 車
	playerCar_ = std::make_unique<PlayerCar>();
	playerCar_->Initialize(entityManager, globalVariables, playerCarPos_, { 0,Math::DegreesToRadians(-90),0});
	playerCar_->SetGroungHeight(0.1f);

	// ステート変更
	playerCar_->GetStateMachine()->ChangeState(CarMainState::ResultCashExchange);
	// ライト
	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 0.9f;
	directionalLightData.isLight = true;
	directionalLightData.lig = 0.1f;
	directional = std::make_shared<Engine::DirectionalLight>();
	directional->directional = directionalLightData;
	entityManager->GetLightManager()->AddLight(directional);
}

void ResultStage::Update(float dt) {
	// プレイヤー車更新
	playerCar_->Update(dt);
}

void ResultStage::Draw() {

}

void ResultStage::DrawP() {
}

void ResultStage::Draw2D() {
}
