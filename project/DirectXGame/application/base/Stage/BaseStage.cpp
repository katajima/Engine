#include "BaseStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"



void BaseStage::InitializeOcean(Engine::EntityManager* entityManager, CameraManager* cameraManager) {
	// 海初期化
	ocean_ = std::make_unique<Engine::Ocean>();
	ocean_->Initialize(entityManager, oceanParameters_.range);
	ocean_->GetWaveParameters()[0].amplitude = oceanParameters_.amplitude;
	ocean_->GetWaveParameters()[0].waveDirection = oceanParameters_.waveDirection;
	ocean_->GetWaveParameters()[0].speed = oceanParameters_.waveSpeed;
	ocean_->GetMaterial()->GetMaterialInstance().color = oceanParameters_.color;

	// 海オブジェクト追加
	oceanObject = entityManager->CreateObject3D("oceanObject", Engine::ObjectModelType::kOcean, {}, cameraManager->GetCamera());
	oceanObject->SetOcean(ocean_.get());
	oceanObject->GetWorldTransform().translate_ = oceanParameters_.translate;
	oceanObject->GetWorldTransform().rotate_ = oceanParameters_.rotate;
	oceanObject->GetRenderComponent()->SetObjectDrawType(Engine::ObjectDrawType::kTranslucent03);
}

void BaseStage::InitializeSky(Engine::EntityManager* entityManager, CameraManager* cameraManager) {
	// スカイボックス初期化
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(entityManager, "resources/Texture/hdr/sky.dds");

	// 空
	sky_ = entityManager->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {}, cameraManager->GetCamera());
	sky_->GetWorldTransform().scale_ = skyBoxScale_;
	sky_->SetSkyBox(skyBox.get());
}

void BaseStage::InitializeLight(Engine::EntityManager* entityManager) {
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

void BaseStage::InitializeLevelData(Engine::EntityManager* entityManager, CameraManager* cameraManager, const std::string& name) {
	// レベルデータロード
	loadData_ = std::make_unique<LoadLevelData>();
	loadData_->SetCameraManager(cameraManager);
	loadData_->Initialize(entityManager, entityManager->GetObject3dCommon()->GetDxCommon()->GetModelManager(), nullptr, name);
}

void BaseStage::InitializeStageColliderSystem(Engine::EntityManager* entityManager) {
	// ステージコライダーシステム初期化
	stageColliderSystem_ = std::make_unique<StageColliderSystem>();
	stageColliderSystem_->Initialize(entityManager->Get3DLineCommon());
	
	// レベルデータだ存在するなら
	if (loadData_) {
		for (auto& obj : loadData_->GetObjects()) {
			stageColliderSystem_->AddObject(obj);
		}
		stageColliderSystem_->CreateCollider();
	}
}

void BaseStage::BaseUpdate(float dt) {
	
	// ステージコライダーシステムがあるなら
	if (stageColliderSystem_) {
		stageColliderSystem_->Update();
	}
	// レベルデータがあるなら更新
	if (loadData_) {
		loadData_->Update();
	}
}
