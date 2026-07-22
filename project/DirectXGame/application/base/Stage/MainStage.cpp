#include "MainStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManager.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

void MainStage::Initialize(Engine::EntityManager* entityManager, Engine::AudioManager* audioManager, CameraManager* cameraManager) {
	this->entityManager = entityManager;	// エンティティ3d
	this->camera = cameraManager->GetCamera();	// カメラ取得
	// 海初期化
	InitializeOcean(entityManager, cameraManager);
	// 空初期化
	InitializeSky(entityManager, cameraManager);
	// レベルデータ初期化
	InitializeLevelData(entityManager, cameraManager, "gameScene.json");
	// ステージコライダーシステム初期化
	InitializeStageColliderSystem(entityManager);

	std::unique_ptr<Engine::CylinderPrimitive> cylinderPrimitive = std::make_unique<Engine::CylinderPrimitive>();
	cylinderPrimitive->Initialize(entityManager->GetPrimitiveCommon(), "resources/Texture/simasima.png", { 1,0,0,0.5f }, "StageLimit");
	cylinderPrimitive->Data().segments = 4;
	cylinderPrimitive->Data().innerRadius = 150.0f;
	cylinderPrimitive->Data().outerRadius = 150.0f;

	cylinderPrimitive->Data().isCover = false;
	cylinderPrimitive->Data().height = 10.0f;

	stageLimit_ = entityManager->CreateObject3D("StageLimit", Engine::ObjectModelType::kPrimitive, { 0,0,0 }, camera);
	stageLimit_->SetPrimitive(std::move(cylinderPrimitive));
	stageLimit_->GetPrimitive()->SetPsoType(Engine::BasePrimitive::PsoType::kNoCullRingClamp);
	stageLimit_->GetWorldTransform().rotate_.x = Math::DegreesToRadians(90);
	stageLimit_->GetWorldTransform().rotate_.y = Math::DegreesToRadians(45);
	// 車
	playerCar_ = std::make_unique<PlayerCar>();
	playerCar_->Initialize(entityManager, audioManager,{},playerCarPos_, { 0,Math::DegreesToRadians(0),0 });
	playerCar_->SetGroundHeight(0.0f);
}

void MainStage::Update(float dt) {
	BaseUpdate(dt);

	stageLimit_->GetPrimitive()->GetMaterial()->GetMaterialInstance().transform.translate.x += dt * 0.10f;

	playerCar_->Update(dt);
}


