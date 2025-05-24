#include "Stage.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

#include "DirectXGame/engine/MyGame/MyGame.h"

void Stage::Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	dxCommon_ = dxcommon;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	skyBox = std::make_unique<SkyBox>();
	skyBox->Initialize(entity3DManager_, "resources/Texture/hdr/sky.dds");

	// 空
	sky_ = std::make_unique<Object3d>();
	sky_->Initialize(entity3DManager_, Object3d::ObjectType::kSkyBox);
	sky_->SetModel("resources/Texture/hdr/sky.dds");
	sky_->SetCamera(camera);
	sky_->worldtransform_.scale_ = { 200,200,200 };
	sky_->SetSkyBox(skyBox.get());
	sky_->SetName("skyBox");

	// 地面
	tail_ = std::make_unique<Object3d>();
	tail_->Initialize(entity3DManager_);
	tail_->SetModel("stage.gltf");
	tail_->SetName("stage");
	tail_->SetCamera(camera);
	tail_->worldtransform_.scale_ = { 4,4,4 };
	tail_->GetMaterial(0)->transform.scale = { 10,10,1 };
	tail_->GetMaterial(0)->shininess_ = 64.0f;

	// 列車
	train_ = std::make_unique<Object3d>();
	train_->Initialize(entity3DManager_);
	train_->SetModel("train.gltf");
	train_->SetName("train");
	train_->SetCamera(camera);
	train_->worldtransform_.translate_ = { -3111,300,1040 };
	train_->worldtransform_.scale_ = { 8,8,8 };
	//train_->GetMaterial(0)->transform.scale = {10,10,1};
	train_->GetMaterial(0)->shininess_ = 64.0f;

	PointLightData pointLightData{};
	pointLightData.color = { 1,1,1,1 };
	pointLightData.position = { 0,100,0 };
	pointLightData.radius = 1000.0f;
	pointLightData.intensity = 1.0f;
	pointLightData.isLight = true;
	pointLightData.lig = 0.1f;


	pointLight_ = std::make_shared<PointLight>();
	pointLight_->point = pointLightData;

	entity3DManager_->GetLightManager()->AddLight(pointLight_);


	emit_ = std::make_unique<ParticleEmitter>();
	emit_->Initialize(particleManager, "groundRtttight", "dustt");
	emit_->GetFrequency() = 0.5f;
	emit_->SetCount(200);
	emit_->SetPos({ 200,40,200 });
	emit_->SetVelocityMinMax(-Vector3{ 0.2f,0.2f,0.2f }, { 0.2f, 0.2f, 0.2f });
	emit_->SetLifeTimeMinMax(10.4f, 10.7f);
	emit_->SetIsAlpha(true);
	emit_->SetSizeMinMax(Vector3{ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f });
	emit_->SetColorMinMax({ 0.604f, 0.384f, 0.161f }, { 0.604f, 0.384f, 0.161f });
	emit_->SetRengeMinMax({ -400,-100,-400 }, { 400,100,400 });


	emitTrainDust_ = std::make_unique<ParticleEmitter>();
	emitTrainDust_->Initialize(particleManager, "trainDust", "trainDust");
	emitTrainDust_->GetFrequency() = 0.0f;
	emitTrainDust_->SetLifeTimeMinMax(2.0f, 3.0f);
	emitTrainDust_->SetSizeMinMax(Vector3{ 10.0f,10.0f,10.0f }, { 10.0f,10.0f,10.0f });
	emitTrainDust_->SetColorMinMax({ 0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f });
	emitTrainDust_->SetAlphaClipping(0.0f);
	emitTrainDust_->SetCount(7);
	emitTrainDust_->transform_.translate_ = { 6,9,0 };
	emitTrainDust_->SetParent(train_->worldtransform_);
	emitTrainDust_->SetIsAlpha(true);
	emitTrainDust_->SetVelocityMinMax(Vector3{ 0.0f,10.0f,0.0f }, { 0.0f, 20.0f, 0.0f });
	//emitTrainDust_->SetIsLifeTimeScale(true);
	//emitTrainDust_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);
	emitTrainDust_->SetUsebillboard(true);
	emitTrainDust_->SetUsebillboardRotZ(true);
	emitTrainDust_->SetRotateMinMax(-Vector3{0,0,3.14f}, Vector3{0,0,3.14f});

}

void Stage::Update()
{
	train_->worldtransform_.translate_.x += trainSpeed_ * MyGame::GameTime();

	if (trainEndX_ < train_->worldtransform_.translate_.x)
	{
		trainWarpTime_ += MyGame::GameTime();

		if (trainWarpTime_ > trainWarpTimeMax_) {
			trainWarpTime_ = 0.0f;
			train_->worldtransform_.translate_.x = trainStartX_;
		}
	}

	emit_->Update();
	emitTrainDust_->Update();
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
