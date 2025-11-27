#include "Stage.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

#include "DirectXGame/engine/MyGame/MyGame.h"

void Stage::Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	dxCommon_ = dxcommon;				// ダイレクトX共通クラス
	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	
	// 海初期化
	ocean_ = std::make_unique<Ocean>();
	ocean_->Initialize(entity3DManager_, provisionalData_.oceanRange);
	ocean_->GetWaveParameters()[0].amplitude = provisionalData_.oceanAmplitude;
	ocean_->GetWaveParameters()[0].waveDirection = provisionalData_.waveDirection;
	ocean_->GetWaveParameters()[0].speed = provisionalData_.waveSpeed;
	ocean_->GetMaterial()->color = provisionalData_.color;

	// 海オブジェクト追加
	oceanObject = entity3DManager_->CreateObject3D("oceanObject", ObjectModelType::kOcean, {}, camera_);
	oceanObject->SetOcean(ocean_.get());
	oceanObject->GetWorldTransform().translate_ = provisionalData_.oceanTranslate;
	oceanObject->GetWorldTransform().rotate_ = provisionalData_.oceanRotate;
	oceanObject->GetRenderComponent()->SetObjectDrawType(ObjectDrawType::kTranslucent03);
	
	// スカイボックス初期化
	skyBox = std::make_unique<SkyBox>();
	skyBox->Initialize(entity3DManager_, "resources/Texture/hdr/sky.dds");
	
	// 空
	sky_ = entity3DManager_->CreateObject3D("skyBox", ObjectModelType::kSkyBox, {},camera_);
	sky_->GetWorldTransform().scale_ = provisionalData_.skyBoxScale;
	sky_->SetSkyBox(skyBox.get());
	
	// ミサイル発射台追加
	for(int i = 0; i < provisionalData_.missileNumX; ++i)
	{
		for(int j = 0; j < provisionalData_.missileNumY; ++j)
		{
			auto object = entity3DManager->CreateObject3D("Missile" + std::to_string(j) + "_" + std::to_string(i),ObjectModelType::kNormal,
				{ provisionalData_.missileTranslate.x + static_cast<float>(j) * provisionalData_.missileInterval.x ,
				  provisionalData_.missileTranslate.y,
				  provisionalData_.missileTranslate.z + static_cast<float>(i) * provisionalData_.missileInterval.y },camera_);
			object->SetModel("Missile.gltf");
			object->GetWorldTransform().rotate_ = provisionalData_.missileRotate;
			
			object->GetWorldTransform().scale_ = { provisionalData_.missileSize,provisionalData_.missileSize,provisionalData_.missileSize };
			missiles_.push_back(object);
		}	
	}

	

	
	
}

void Stage::Update()
{
	
}

void Stage::Draw()
{

}

void Stage::DrawP()
{

}

void Stage::Draw2D()
{

}

void Stage::InitEmit()
{
	
}

void Stage::EmitUpdate()
{
	
}


