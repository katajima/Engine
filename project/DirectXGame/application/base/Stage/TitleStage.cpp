#include "TitleStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManager.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

void TitleStage::InitializeStone() {
	stone1_ = entityManager->CreateObject3D("Stone1", Engine::ObjectModelType::kNormal, {}, camera);
	stone1_->SetModel("Stone.obj");
	stone1_->GetWorldTransform().scale_ = { 1,1,1 };
	stone1_->GetWorldTransform().translate_ = { -260,-20,700 };

	stone2_ = entityManager->CreateObject3D("Stone2", Engine::ObjectModelType::kNormal, {}, camera);
	stone2_->SetModel("Stone.obj");
	stone2_->GetWorldTransform().scale_ = { 1,1,1 };
	stone2_->GetWorldTransform().translate_ = { 260,-20,500 };

	stone3_ = entityManager->CreateObject3D("Stone3", Engine::ObjectModelType::kNormal, {}, camera);
	stone3_->SetModel("Stone.obj");
	stone3_->GetWorldTransform().scale_ = { 1,1,1 };
	stone3_->GetWorldTransform().translate_ = { -160,-25,1000 };

	stone4_ = entityManager->CreateObject3D("Stone4", Engine::ObjectModelType::kNormal, {}, camera);
	stone4_->SetModel("Stone.obj");
	stone4_->GetWorldTransform().scale_ = { 1,1,1 };
	stone4_->GetWorldTransform().translate_ = { 200,-28,900 };

	stone5_ = entityManager->CreateObject3D("Stone5", Engine::ObjectModelType::kNormal, {}, camera);
	stone5_->SetModel("Stone.obj");
	stone5_->GetWorldTransform().scale_ = { 1,1,1 };
	stone5_->GetWorldTransform().translate_ = { 0,-28,2000 };
};

void TitleStage::Initialize(Engine::EntityManager* entityManager, Engine::AudioManager* audioManager, CameraManager* cameraManager) {
	this->entityManager = entityManager;	// エンティティ3d
	this->camera = cameraManager->GetCamera();	// カメラ取得
	// 空初期化
	InitializeSky(entityManager, cameraManager);
	// ライト初期化
	InitializeLight(entityManager);
	// 石初期化
	InitializeStone();

	// タイル
	tail = entityManager->CreateObject3D("tail", Engine::ObjectModelType::kNormal, {}, camera);
	tail->SetModel("Ground.obj");
	tail->GetWorldTransform().scale_ = { 100,100,100 };
	tail->GetMaterial(0)->GetMaterialInstance().transform.scale = { 100,100,100 };

	// 車
	playerCar_ = std::make_unique<PlayerCar>();
	playerCar_->Initialize(entityManager, audioManager,{}, playerCarPos_, { 0,Math::DegreesToRadians(-90),0 });
	playerCar_->SetGroundHeight(0.1f);

}

void TitleStage::Update(float dt) {
	BaseUpdate(dt);

	// エフェクト
	effect->Emit("EmitterDust", { -50,10,100 });
	effect->Emit("EmitterDust", { -50,10,100 });


	playerCar_->Update(dt);
};

