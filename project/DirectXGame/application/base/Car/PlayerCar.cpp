#include "PlayerCar.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/Math/Random.h"
#include "DirectXGame/application/base/Effect/Effect.h"


void PlayerCar::Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, const Vector3& pos, const Vector3& rotate) {
	this->entity3DManager = entity3DManager;
	this->globalVariables = globalVariables;
	pos_ = pos;
	// 車体
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager, globalVariables,"TrackCarBoby","TrackCarBody.obj",false,false,nullptr);
	objectComponent_->SetSRT({1,1,1}, rotate, pos_);


	// タイヤ
	objectComponentTire01_ = std::make_unique<ObjectComponent>();
	objectComponentTire01_->Initialize(entity3DManager, globalVariables, "TrackCarTire01", "TrackCarTire.obj", false, false, nullptr);
	objectComponentTire01_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectComponentTire01_->GetWorldTransform().translate_ = { 1.0f ,0.3f,-1.3f};

	// タイヤ
	objectComponentTire02_ = std::make_unique<ObjectComponent>();
	objectComponentTire02_->Initialize(entity3DManager, globalVariables, "TrackCarTire02", "TrackCarTire.obj", false, false, nullptr);
	objectComponentTire02_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectComponentTire02_->GetWorldTransform().translate_ = { 1.0f ,0.3f, 2.3f };

	// タイヤ
	objectComponentTire03_ = std::make_unique<ObjectComponent>();
	objectComponentTire03_->Initialize(entity3DManager, globalVariables, "TrackCarTire03", "TrackCarTire.obj", false, false, nullptr);
	objectComponentTire03_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectComponentTire03_->GetWorldTransform().translate_ = { -1.0f ,0.3f,-1.3f };

	// タイヤ
	objectComponentTire04_ = std::make_unique<ObjectComponent>();
	objectComponentTire04_->Initialize(entity3DManager, globalVariables, "TrackCarTire04", "TrackCarTire.obj", false, false, nullptr);
	objectComponentTire04_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectComponentTire04_->GetWorldTransform().translate_ = { -1.0f ,0.3f,2.3f };

	// オブジェクトコンポーネント追加
	objectComponentShadow_ = std::make_unique<ObjectComponent>();
	// オブジェクトインスタンシング初期化
	objectComponentShadow_->InitializeInstancing(entity3DManager, globalVariables, "PlayerBase2", "plane.obj", "resources/Texture/smoke/no4.dds",
		false, false, nullptr, Engine::Object3dInstansManager::TransparencyType::kYes);
	
	objectComponentShadow_->SetInstancingSRT({ 5.0f,5.0f,5.0f }, { Math::DegreesToRadians(-90),0.0f,0.0f }, { 0.0f,0.1f,0.0f });
	objectComponentShadow_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効


	spotLightTransform01_.Initialize();
	spotLightTransform01_.parent_ = &objectComponent_->GetWorldTransform();
	spotLightTransform01_.translate_ = { -1.0f ,0.8f,-1.5f };
	spotLightTransform02_.Initialize();
	spotLightTransform02_.parent_ = &objectComponent_->GetWorldTransform();
	spotLightTransform02_.translate_ = { 1.0f ,0.8f,-1.5f };




	SpotLightData spotData{};
	spotData.color = { 1,1,1,1 };
	spotData.direction = { 1,0,0 };
	spotData.isLight = false;
	spotData.distance = 5.0f;
	spotData.intensity = 2.0f;
	spotData.position = pos_ + Vector3{ 0.0f,1.0f,0.0f };
	spotData.cosAngle = 0.72f;
	spotData.decay = 0.8f;
	spotData.cosFalloffStart = 0.9f;

	spotLight01_ = std::make_shared<Engine::SpotLight>();
	spotLight01_->spot = spotData;
	entity3DManager->GetLightManager()->AddLight(spotLight01_);
	spotLight02_ = std::make_shared<Engine::SpotLight>();
	spotLight02_->spot = spotData;
	entity3DManager->GetLightManager()->AddLight(spotLight02_);


}

void PlayerCar::Update(float dt) {


	if (isMoving_) {
		moveTimer_ += dt;

		
		if (moveTimer_ >= 0.3f) {
			spotLight01_->spot.isLight = true;
			spotLight02_->spot.isLight = true;
		}


		if (moveTimer_ >= 0.5f) {
			velocity_.x += 0.5f;

		

			objectComponent_->GetWorldTransform().translate_.x += velocity_.x * dt;
			objectComponent_->GetWorldTransform().translate_.y = pos_.y;

			objectComponentTire01_->GetWorldTransform().rotate_.x += velocity_.x * dt * 0.1f;
			objectComponentTire02_->GetWorldTransform().rotate_.x += velocity_.x * dt * 0.1f;
			objectComponentTire03_->GetWorldTransform().rotate_.x += velocity_.x * dt * 0.1f;
			objectComponentTire04_->GetWorldTransform().rotate_.x += velocity_.x * dt * 0.1f;


			effect->Emit("dust2", objectComponentTire01_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,-0.1f,0 });
			effect->Emit("dust2", objectComponentTire02_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,-0.1f,0 });
			effect->Emit("dust2", objectComponentTire03_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,-0.1f,0 });
			effect->Emit("dust2", objectComponentTire04_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,-0.1f,0 });
		}
		else {
			objectComponent_->GetWorldTransform().translate_.y = pos_.y + Random::RandomFloat(-0.03f, 0.03f);
		}
	}
	


	objectComponent_->Update();


	spotLightTransform01_.Update();
	spotLightTransform02_.Update();

	spotLight01_->spot.position = spotLightTransform01_.GetWorldPosition();
	spotLight02_->spot.position = spotLightTransform02_.GetWorldPosition();


	objectComponentTire01_->Update();
	objectComponentTire02_->Update();
	objectComponentTire03_->Update();
	objectComponentTire04_->Update();


	objectComponentShadow_->GetWorldTransform().translate_.x = objectComponent_->GetWorldTransform().translate_.x + -0.5f;
	objectComponentShadow_->GetWorldTransform().translate_.z = objectComponent_->GetWorldTransform().translate_.z + 0.0f;
	objectComponentShadow_->GetWorldTransform().translate_.y = height_;

	objectComponentShadow_->Update();
}