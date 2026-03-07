#include "MainStage.h"

#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"



void MainStage::Initialize(Engine::DirectXCommon* dxcommon, Engine::EntityManager* entityManager,  Engine::Camera* camera)
{
	this->dxCommon = dxcommon;				// ダイレクトX共通クラス
	this->entityManager = entityManager;	// エンティティ3d
	
	// 海初期化
	ocean_ = std::make_unique<Engine::Ocean>();
	ocean_->Initialize(entityManager, provisionalData_.oceanRange);
	ocean_->GetWaveParameters()[0].amplitude = provisionalData_.oceanAmplitude;
	ocean_->GetWaveParameters()[0].waveDirection = provisionalData_.waveDirection;
	ocean_->GetWaveParameters()[0].speed = provisionalData_.waveSpeed;
	ocean_->GetMaterial()->GetMaterialInstance().color = provisionalData_.color;

	// 海オブジェクト追加
	oceanObject = entityManager->CreateObject3D("oceanObject", Engine::ObjectModelType::kOcean, {}, camera);
	oceanObject->SetOcean(ocean_.get());
	oceanObject->GetWorldTransform().translate_ = provisionalData_.oceanTranslate;
	oceanObject->GetWorldTransform().rotate_ = provisionalData_.oceanRotate;
	oceanObject->GetRenderComponent()->SetObjectDrawType(Engine::ObjectDrawType::kTranslucent03);
	
	// スカイボックス初期化
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(entityManager, "resources/Texture/hdr/sky.dds");
	
	// 空
	sky_ = entityManager->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {},camera);
	sky_->GetWorldTransform().scale_ = provisionalData_.skyBoxScale;
	sky_->SetSkyBox(skyBox.get());
	
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

void MainStage::Update(float dt)
{
	playerCar_->Update(dt);
}

void MainStage::Draw()
{

}

void MainStage::DrawP()
{

}

void MainStage::Draw2D()
{

}

void MainStage::InitEmit()
{
	
}

void MainStage::EmitUpdate()
{
	
}


