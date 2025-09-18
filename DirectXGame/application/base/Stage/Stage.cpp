#include "Stage.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

#include "DirectXGame/engine/MyGame/MyGame.h"

void Stage::Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	dxCommon_ = dxcommon;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	

	ocean_ = std::make_unique<Ocean>();
	ocean_->Initialize(entity3DManager_, { 8000,8000 });
	ocean_->GetWaveParameters()[0].amplitude = 16.5f;
	ocean_->GetWaveParameters()[0].waveDirection = {0.5f,0.5f};
	ocean_->GetWaveParameters()[0].speed = 5.0f;
	ocean_->GetMaterial()->color = Color{ 0.0f, 0.0f, 0.8f, 0.75f };

	oceanObject = entity3DManager_->CreateObject3D("oceanObject", ObjectModelType::kOcean, { 0,-30,0 }, camera_);
	oceanObject->SetOcean(ocean_.get());
	oceanObject->GetWorldTransform().translate_ = { 0,-30,0 };
	oceanObject->GetWorldTransform().rotate_.x = DegreesToRadians(90);
	oceanObject->GetRenderComponent()->SetObjectDrawType(ObjectDrawType::kTranslucent03);
	
	skyBox = std::make_unique<SkyBox>();
	skyBox->Initialize(entity3DManager_, "resources/Texture/hdr/sky.dds");
	
	// 空
	sky_ = entity3DManager_->CreateObject3D("skyBox", ObjectModelType::kSkyBox, {},camera_);
	sky_->GetWorldTransform().scale_ = { 100,100,100 };
	sky_->SetSkyBox(skyBox.get());
	
	//
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			auto object = entity3DManager->CreateObject3D("Missile" + std::to_string(j) + "_" + std::to_string(i),ObjectModelType::kNormal,
				{ 3500 + static_cast<float>(j) * 120.0f ,106,3000 + static_cast<float>(i) * 100.0f },camera_);
			object->SetModel("Missile.gltf");
			object->GetWorldTransform().rotate_.y = DegreesToRadians(-90);
			float size = 10.0f;
			object->GetWorldTransform().scale_ = { size,size,size };
			missiles_.push_back(object);
		}	
	}

	

	
	
	//// 列車
	//train_ = entity3DManager_->CreateObject3D("train", Object3d::ObjectModelType::kNormal, { -3111,300,1040 }, camera_);
	//train_->SetModel("train.gltf");
	//train_->worldtransform_.scale_ = { 8,8,8 };
	//train_->GetMaterial(0)->shininess_ = 64.0f;
	//train_->worldtransform_.Update();
	//// 船
	//ship_ = entity3DManager_->CreateObject3D("ship", Object3d::ObjectModelType::kNormal, { -3111,-50,2040 }, camera_);
	//ship_->SetModel("ship.gltf");
	//ship_->worldtransform_.scale_ = { 8,8,8 };
	//ship_->GetMaterial(0)->shininess_ = 64.0f;
	//ship_->worldtransform_.Update();
	




	

	
	//DirectionalLightData directionalLightData{};
	//directionalLightData.color = { 1,1,1,1 };
	//directionalLightData.direction = { 0,-1,0 };
	//directionalLightData.intensity = 0.5f;
	//directionalLightData.isLight = true;
	//directionalLightData.lig = 0.1f;


	//directional = std::make_shared<DirectionalLight>();
	//directional->directional = directionalLightData;

	//entity3DManager_->GetLightManager()->AddLight(directional);
	
	// エミッター設定
	//InitEmit();
}

void Stage::Update()
{
	/*train_->worldtransform_.translate_.x += trainSpeed_ * MyGame::GameTime();

	if (trainEndX_ < train_->worldtransform_.translate_.x)
	{
		trainWarpTime_ += MyGame::GameTime();

		if (trainWarpTime_ > trainWarpTimeMax_) {
			trainWarpTime_ = 0.0f;
			train_->worldtransform_.translate_.x = trainStartX_;
		}
	}*/

	//pointLight_->point.color = { 1,0,0,1 };

	//velocity_ = Subtract(ship_->GetWorldPosition(), ship_->GetPreWorldPosition());
	
	// static float angle = 0.0f; // 現在の角度
	//// 時間を加味して角度を進める
	//angle += angularSpeed * MyGame::GameTime();
	//ship_->worldtransform_.translate_.x = center.x + (cosf(angle) * radius);
	//ship_->worldtransform_.translate_.z = center.z + (sinf(angle) * radius);
	//
	//// Y軸周り角度(θy)
	//ship_->worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	//float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	//// X軸周り角度(θx)
	//ship_->worldtransform_.rotate_.x = std::atan2(velocity_.y, length);
	



	//EmitUpdate();
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
	/*ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	emit_ = std::make_unique<ParticleEmitter>();
	emit_->Initialize(particleManager, "groundRtttight", "dustt");
	emit_->GetFrequency() = 0.5f;
	emit_->SetCount(200);
	emit_->SetPos({ 0,0,0 });
	emit_->SetVelocityMinMax(-Vector3{ 0.2f,0.2f,0.2f }, { 0.2f, 0.2f, 0.2f });
	emit_->SetLifeTimeMinMax(10.4f, 10.7f);
	emit_->SetIsAlpha(true);
	emit_->SetSizeMinMax(Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	emit_->SetColorMinMax({ 0.604f, 0.384f, 0.161f }, { 0.604f, 0.384f, 0.161f });
	emit_->SetRengeMinMax({ -400,0,-400 }, { 400,100,400 });

	

	emitTrainDust_ = std::make_unique<ParticleEmitter>();
	emitTrainDust_->Initialize(particleManager, "trainDust", "trainDust");
	emitTrainDust_->GetFrequency() = 0.0f;
	emitTrainDust_->SetLifeTimeMinMax(2.0f, 3.0f);
	emitTrainDust_->SetSizeMinMax(Vector3{ 10.0f,10.0f,10.0f }, { 10.0f,10.0f,10.0f });
	emitTrainDust_->SetColorMinMax({ 0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f });
	emitTrainDust_->SetAlphaClipping(0.0f);
	emitTrainDust_->SetCount(7);
	emitTrainDust_->transform_.translate_ = { 6,9,0 };
	emitTrainDust_->SetParent(train_->GetWorldTransform());
	emitTrainDust_->SetIsAlpha(true);
	emitTrainDust_->SetVelocityMinMax(Vector3{ 0.0f,10.0f,0.0f }, { 0.0f, 20.0f, 0.0f });
	emitTrainDust_->SetUsebillboard(true);
	emitTrainDust_->SetUsebillboardRotZ(true);
	emitTrainDust_->SetRotateMinMax(-Vector3{ 0,0,3.14f }, Vector3{ 0,0,3.14f });


	emitShipDust_ = std::make_unique<ParticleEmitter>();
	emitShipDust_->Initialize(particleManager, "shipDust", "shipDust");
	emitShipDust_->GetFrequency() = 0.0f;
	emitShipDust_->SetLifeTimeMinMax(2.0f, 3.0f);
	emitShipDust_->SetSizeMinMax(Vector3{ 50.0f,50.0f,10.0f }, { 50.0f,50.0f,10.0f });
	emitShipDust_->SetColorMinMax({ 1.0f, 1.0f, 1.0f,1.0f }, { 1.0f, 1.0f, 1.0f ,1.0f});
	emitShipDust_->SetRengeMinMax({ -100,-5,-40 }, { 100,5,40 });
	emitShipDust_->SetAlphaClipping(0.0f);
	emitShipDust_->SetCount(7);
	emitShipDust_->transform_.translate_ = { 0,5,0 };
	emitShipDust_->SetParent(ship_->GetWorldTransform());
	emitShipDust_->SetIsAlpha(true);
	emitShipDust_->SetIsGravity(true);
	emitShipDust_->SetVelocityMinMax(Vector3{ 0.0f,0.0f,0.0f }, { 0.0f, 00.0f, 0.0f });
	emitShipDust_->SetUsebillboard(true);
	emitShipDust_->SetUsebillboardRotZ(true);
	emitShipDust_->SetRotateMinMax(-Vector3{ 0,0,3.14f }, Vector3{ 0,0,3.14f });


	emitTrainDust_->Update();

	emitCloudDust_ = std::make_unique<ParticleEmitter>();
	emitCloudDust2_ = std::make_unique<ParticleEmitter>();
	emitCloudDust3_ = std::make_unique<ParticleEmitter>();
	CloudEmit(particleManager,emitCloudDust_.get(), "cloudDust");
	CloudEmit(particleManager,emitCloudDust2_.get(), "cloudDust2");
	CloudEmit(particleManager,emitCloudDust3_.get(), "cloudDust3");*/

}

//void Stage::CloudEmit(ParticleManager* particleManager, ParticleEmitter* emit, const std::string& name)
//{
//	emit->Initialize(particleManager, name, name);
//	emit->GetFrequency() = 0.5f;
//	emit->SetLifeTimeMinMax(10.0f, 20.0f);
//	emit->SetSizeMinMax(Vector3{ 50.0f,50.0f,10.0f }, { 50.0f,50.0f,10.0f });
//	emit->SetColorMinMax({ 1.0f, 1.0f, 1.0f,0.8f }, { 1.0f, 1.0f, 1.0f ,0.8f });
//	emit->SetRengeMinMax({ -4000,-5,-4000 }, { 4000,100,4000 });
//	emit->SetAlphaClipping(0.0f);
//	emit->SetCount(10);
//	emit->transform_.translate_ = { -2000,1500,0 };
//	emit->SetIsAlpha(true);
//	emit->SetVelocityMinMax(Vector3{ 20.0f,0.0f,0.0f }, { 60.0f, 00.0f, 0.0f });
//	emit->SetUsebillboard(true);
//	emit->SetUsebillboardRotZ(true);
//	emit->SetRotateMinMax(-Vector3{ 0,0,0.0f }, Vector3{ 0,0,0.0f });
//}

void Stage::EmitUpdate()
{
	// 埃のパーティクルエミッターの更新
	//emit_->Update();

	// 列車のパーティクルエミッター
	//emitTrainDust_->Update();

	// 船のパーティクルエミッター
	//emitShipDust_->SetVelocityMinMax(-velocity_ * 10, -velocity_ * 10);
	//emitShipDust_->Update();

	// 雲のパーティクルエミッターの更新
	//emitCloudDust_->Update();
	//emitCloudDust2_->Update();
	//emitCloudDust3_->Update();
}


