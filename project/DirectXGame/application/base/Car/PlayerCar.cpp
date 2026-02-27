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
	objectComponent_->Initialize(entity3DManager, globalVariables, "TrackCarBoby", "TrackCarBody.obj", false, false, nullptr);
	objectComponent_->SetSRT({ 1,1,1 }, rotate, pos_);


	// タイヤ
	objectLFTier_ = std::make_unique<ObjectComponent>();
	objectLFTier_->Initialize(entity3DManager, globalVariables, "TrackCarTireLF", "TrackCarTire.obj", false, false, nullptr);
	objectLFTier_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectLFTier_->GetWorldTransform().translate_ = { 1.0f ,0.3f,-1.3f };

	//// タイヤ
	objectLBTire_ = std::make_unique<ObjectComponent>();
	objectLBTire_->Initialize(entity3DManager, globalVariables, "TrackCarTireLB", "TrackCarTire.obj", false, false, nullptr);
	objectLBTire_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectLBTire_->GetWorldTransform().translate_ = { 1.0f ,0.3f, 2.0f };

	//// タイヤ
	objectRFTire_ = std::make_unique<ObjectComponent>();
	objectRFTire_->Initialize(entity3DManager, globalVariables, "TrackCarTireRF", "TrackCarTire.obj", false, false, nullptr);
	objectRFTire_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectRFTire_->GetWorldTransform().translate_ = { -1.0f ,0.3f,-1.3f };

	//// タイヤ
	objectRBTire_ = std::make_unique<ObjectComponent>();
	objectRBTire_->Initialize(entity3DManager, globalVariables, "TrackCarTireRB", "TrackCarTire.obj", false, false, nullptr);
	objectRBTire_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	objectRBTire_->GetWorldTransform().translate_ = { -1.0f ,0.3f,2.0f };

	// オブジェクトコンポーネント追加
	objectComponentShadow_ = std::make_unique<ObjectComponent>();
	// オブジェクトインスタンシング初期化
	objectComponentShadow_->InitializeInstancing(entity3DManager, globalVariables, "PlayerBase2", "plane.obj", "resources/Texture/smoke/no4.dds",
		false, false, nullptr, Engine::Object3dInstansManager::TransparencyType::kYes);

	objectComponentShadow_->SetInstancingSRT({ 5.0f,5.0f,5.0f }, { Math::DegreesToRadians(-90),0.0f,0.0f }, { 0.0f,0.1f,0.0f });
	objectComponentShadow_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効

	// スポットライト

	spotLightTransformRF_.Initialize();
	spotLightTransformRF_.parent_ = &objectComponent_->GetWorldTransform();
	spotLightTransformRF_.translate_ = { -1.0f ,0.8f,-1.5f };
	spotLightTransformLF_.Initialize();
	spotLightTransformLF_.parent_ = &objectComponent_->GetWorldTransform();
	spotLightTransformLF_.translate_ = { 1.0f ,0.8f,-1.5f };




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

	spotLightRF_ = std::make_shared<Engine::SpotLight>();
	spotLightRF_->spot = spotData;
	entity3DManager->GetLightManager()->AddLight(spotLightRF_);
	spotLightLF_ = std::make_shared<Engine::SpotLight>();
	spotLightLF_->spot = spotData;
	entity3DManager->GetLightManager()->AddLight(spotLightLF_);

	// ステートマシン初期化
	stateMachine_ = std::make_unique<PlayerCarStateMachine>();
	stateMachine_->RegisterState(CarMainState::Idle, [](PlayerCar* p) {
		return std::make_unique<CarIdleState>(CarMainState::Idle,p);
		}); 
	stateMachine_->RegisterState(CarMainState::Move, [](PlayerCar* p) {
		return std::make_unique<CarMoveState>(CarMainState::Move,p);
		});
	stateMachine_->RegisterState(CarMainState::PreparationMove, [](PlayerCar* p) {
		return std::make_unique<CarPreparationMoveState>(CarMainState::PreparationMove, p);
		});
	stateMachine_->RegisterState(CarMainState::SelectMove, [](PlayerCar* p) {
		return std::make_unique<CarSelectMove>(CarMainState::SelectMove, p);
		});
	stateMachine_->Initialize(this);

}

void PlayerCar::Update(float dt) {
	// ステートマシン更新
	stateMachine_->Update(dt);


	if (isMoving_) {
		moveTimer_ += dt;
		if (moveTimer_ >= 0.5f) {
			stateMachine_->ChangeState(CarMainState::Move);
		}
		else {
			stateMachine_->ChangeState(CarMainState::PreparationMove);
		}
	}

	objectComponent_->Update();

	// タイヤの更新
	TireUpdate(dt);
	// スポットライトの更新
	SpotLightUpdate(dt);
	// 影の更新
	ShadowUpdate(dt);
}

void PlayerCar::ShadowUpdate(float dt) {
	// 影の位置を車体の位置に合わせる
	objectComponentShadow_->GetWorldTransform().translate_.x = objectComponent_->GetWorldTransform().translate_.x + -0.5f;
	objectComponentShadow_->GetWorldTransform().translate_.z = objectComponent_->GetWorldTransform().translate_.z + 0.0f;
	objectComponentShadow_->GetWorldTransform().translate_.y = height_;

	objectComponentShadow_->Update();
}

void PlayerCar::TireUpdate(float dt){
	objectLFTier_->Update();
	objectLBTire_->Update();
	objectRFTire_->Update();
	objectRBTire_->Update();
}

void PlayerCar::SpotLightUpdate(float dt){
	spotLightTransformRF_.Update();
	spotLightTransformLF_.Update();
	spotLightRF_->spot.position = spotLightTransformRF_.GetWorldPosition();
	spotLightLF_->spot.position = spotLightTransformLF_.GetWorldPosition();
}

void PlayerCar::Emit(const Vector3& pos) {
	effect->Emit("dust2", pos);
}

void PlayerCar::Emit(const Vector3& pos, const Vector3& dir, const Vector3& range) {
	effect->Emit("dust2", pos, dir, range);
}
