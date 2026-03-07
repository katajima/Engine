#include "SelectStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/engine/Math/Random.h"

void SelectStage::Initialize(Engine::EntityManager* entityManager, 
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

	// 車
	playerCar_ = std::make_unique<PlayerCar>();
	playerCar_->Initialize(entityManager, globalVariables, playerCarPos_, { 0,0,0 });
	playerCar_->SetGroungHeight(0.1f);

	

	for (int i = 0; i < 300; i++) {
		std::unique_ptr<ObjectComponent> stone = std::make_unique<ObjectComponent>();
		stone->InitializeInstancing(entityManager, 
			globalVariables, "Stone" + std::to_string(i), "Stone.obj", "",false, false, nullptr,
			Engine::Object3dInstansManager::TransparencyType::kNo);
		stone->GetWorldTransform().translate_ = { Random::RandomFloat(-6000, -400.0f) , -20.0f, Random::RandomFloat(-100, 7000.0f) };
		stone->GetWorldTransform().rotate_.y = Random::RandomFloat(0, 360.0f);
		stone->GetWorldTransform().scale_ = Random::RandomFloat(0.5f,2.25f);
		stoneComponents_.push_back(std::move(stone));
	}

	

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

void SelectStage::Update(float dt) {
	tail->GetMaterial(0)->GetMaterialInstance().transform.translate.y += 0.05f * dt;
	if (tail->GetMaterial(0)->GetMaterialInstance().transform.translate.y >= 1.0f) {
		tail->GetMaterial(0)->GetMaterialInstance().transform.translate.y = 0.0f;
	}

	// プレイヤー車更新
	playerCar_->Update(dt);


	for(auto& stone : stoneComponents_){
		stone->GetWorldTransform().translate_.z += 20.0f;
		if (stone->GetWorldTransform().translate_.z >= 7000.0f) {
			
			stone->GetWorldTransform().translate_.x = Random::RandomFloat(-6000, -400.0f);

			stone->GetWorldTransform().translate_.z = 0;
			stone->GetWorldTransform().rotate_.y = Random::RandomFloat(0, 360.0f);
			stone->GetWorldTransform().scale_ = Random::RandomFloat(0.5f, 2.25f);

		}
		stone->Update();
	}
}

void SelectStage::Draw() {

}

void SelectStage::DrawP() {

}
