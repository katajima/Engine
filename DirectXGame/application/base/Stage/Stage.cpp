#include "Stage.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

void Stage::Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	dxCommon_ =  dxcommon;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();


	// 空
	sky_ = std::make_unique<Object3d>();
	sky_->Initialize(entity3DManager_);
	sky_->SetModel("skydome.obj");
	sky_->SetCamera(camera);
	sky_->worldtransform_.scale_ = { 100,100,100 };
	sky_->model->modelData.material[0]->enableLighting_ = false;

	// 地面
	tail_ = std::make_unique<Object3d>();
	tail_->Initialize(entity3DManager_);
	tail_->SetModel("renga.gltf");
	//tail_->SetModel("coast.gltf");
	tail_->SetCamera(camera);
	tail_->worldtransform_.scale_ = { 4,1,4 };
	tail_->model->modelData.material[0]->transform.scale = {10,10,1};
	tail_->model->modelData.material[0]->shininess_ = 1064.0f;
	

	// 地面黒
	tail2_ = std::make_unique<Object3d>();
	tail2_->Initialize(entity3DManager_);
	tail2_->SetModel("black.obj");
	tail2_->SetCamera(camera);
	tail2_->worldtransform_.scale_ = { 104,104,104 };
	tail2_->worldtransform_.translate_.y = -20;


	// 工場
	warePos.push_back({ 300,0,-200 });
	warePos.push_back({ 300,0,-100 });
	warePos.push_back({ 300,0,0 });
	warePos.push_back({ 300,0,100 });
	warePos.push_back({ 300,0,200 });

	for (int i = 0; i < warePos.size(); i++) {
		auto obj = std::make_unique<Object3d>();
		obj->Initialize(entity3DManager_);
		obj->SetModel("warehouse.gltf");
		obj->SetCamera(camera);
		obj->worldtransform_.scale_ = { 2, 2, 2 };
		obj->worldtransform_.translate_ = warePos[i];
		obj->worldtransform_.rotate_.y = DegreesToRadians(90);


		warehouseObject_.push_back(std::move(obj));
	}

	// 海
	ocean_ = std::make_unique<Ocean>();
	ocean_->Initialize(entity3DManager_, { 10000,10000 });
	ocean_->SetCamera(camera);
	ocean_->transform.rotate.x = DegreesToRadians(90);
	ocean_->transform.translate.y = -10;
	ocean_->material->color = { 0,0,0.57f,1 };
	ocean_->material->color.a = 0.95f;



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
}

void Stage::Update()
{
	// タイル
	tail_->Update();
	tail2_->Update();

	//空
	sky_->Update();


	// 工場
	for (int i = 0; i < warehouseObject_.size(); i++) {
		warehouseObject_[i]->Update();
	}

	// 海
	ocean_->material->color.a = 0.75f;
	ocean_->Update();


	emit_->Update();
}

void Stage::Draw()
{
	// 空
	sky_->Draw();

	// 地面
	tail_->Draw();
	tail2_->Draw();

	// 工場
	for (int i = 0; i < warehouseObject_.size(); i++) {
		warehouseObject_[i]->Draw();
	}

}

void Stage::DrawP()
{
	ocean_->Draw();
}

void Stage::Draw2D()
{

}
