#include "MainStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

void MainStage::Initialize(Engine::EntityManager* entityManager, CameraManager* cameraManager) {
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

	// ミサイル発射台追加
	for(int i = 0; i < provisionalData_.missileNumX; ++i)
	{
		for(int j = 0; j < provisionalData_.missileNumY; ++j)
		{
			auto object = entityManager->CreateObject3D("Missile" + std::to_string(j) + "_" + std::to_string(i), Engine::ObjectModelType::kNormal,
				{ provisionalData_.missileTranslate.x + static_cast<float>(j) * provisionalData_.missileInterval.x ,
				  provisionalData_.missileTranslate.y,
				  provisionalData_.missileTranslate.z + static_cast<float>(i) * provisionalData_.missileInterval.y },camera);
			object->SetModel("Missile.gltf");
			object->GetWorldTransform().rotate_ = provisionalData_.missileRotate;
			
			object->GetWorldTransform().scale_ = { provisionalData_.missileSize,provisionalData_.missileSize,provisionalData_.missileSize };
			missiles_.push_back(object);
		}	
	}

	
	// 車
	playerCar_ = std::make_unique<PlayerCar>();
	playerCar_->Initialize(entityManager, {},playerCarPos_, { 0,Math::DegreesToRadians(0),0 });
	playerCar_->SetGroungHeight(-2.9f);	
}

void MainStage::Update(float dt) {
	BaseUpdate(dt);

	playerCar_->Update(dt);
}


