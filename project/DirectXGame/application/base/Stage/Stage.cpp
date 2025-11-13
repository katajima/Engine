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
	ocean_->Initialize(entity3DManager_, { 8000,8000 });
	ocean_->GetWaveParameters()[0].amplitude = 16.5f;
	ocean_->GetWaveParameters()[0].waveDirection = {0.5f,0.5f};
	ocean_->GetWaveParameters()[0].speed = 5.0f;
	ocean_->GetMaterial()->color = Color{ 0.0f, 0.0f, 0.8f, 0.75f };

	// 海オブジェクト追加
	oceanObject = entity3DManager_->CreateObject3D("oceanObject", ObjectModelType::kOcean, { 0,-30,0 }, camera_);
	oceanObject->SetOcean(ocean_.get());
	oceanObject->GetWorldTransform().translate_ = { 0,-30,0 };
	oceanObject->GetWorldTransform().rotate_.x = Math::DegreesToRadians(90);
	oceanObject->GetRenderComponent()->SetObjectDrawType(ObjectDrawType::kTranslucent03);
	
	// スカイボックス初期化
	skyBox = std::make_unique<SkyBox>();
	skyBox->Initialize(entity3DManager_, "resources/Texture/hdr/sky.dds");
	
	// 空
	sky_ = entity3DManager_->CreateObject3D("skyBox", ObjectModelType::kSkyBox, {},camera_);
	sky_->GetWorldTransform().scale_ = { 100,100,100 };
	sky_->SetSkyBox(skyBox.get());
	
	// ミサイル発射台追加
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			auto object = entity3DManager->CreateObject3D("Missile" + std::to_string(j) + "_" + std::to_string(i),ObjectModelType::kNormal,
				{ 3500 + static_cast<float>(j) * 120.0f ,106,3000 + static_cast<float>(i) * 100.0f },camera_);
			object->SetModel("Missile.gltf");
			object->GetWorldTransform().rotate_.y = Math::DegreesToRadians(-90);
			float size = 10.0f;
			object->GetWorldTransform().scale_ = { size,size,size };
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


