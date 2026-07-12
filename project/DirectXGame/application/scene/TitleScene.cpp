#include"TitleScene.h"
#include <DirectXGame/engine/Audio/Audio.h>


namespace {
	// タイトル画面で選択する既定プレイヤーID。
	constexpr int kDefaultPlayerId = 1;
	// タイトルキャラクターの初期座標。
	const Vector3 kTitlePlayerPosition = { 0.0f, 0.0f, 6.0f };
	// タイトルカメラの初期座標。
	const Vector3 kTitleCameraPosition = { 0.0f, 3.7f, -4.5f };
	// アニメーションを切り替える際の補間時間。
	constexpr float kAnimationBlendSeconds = 0.1f;
	// 車へ乗り込んだ後にセレクト画面へ遷移するまでの時間。
	constexpr float kSceneTransitionDelaySeconds = 1.0f;
	// キャラクターが車へ近づく1フレーム当たりの移動量。
	constexpr float kPlayerApproachSpeedPerFrame = 0.1f;
	// 影オブジェクトを配置する地面からの高さ。
	constexpr float kShadowGroundOffset = 0.1f;
	// 影モデルを地面へ寝かせるX軸回転角度。
	constexpr float kShadowRotationDegrees = -90.0f;
}

void TitleScene::Initialize()
{
	// タイトル画面でも共通入力を利用し、キーボードとコントローラーを同じ操作へ変換する。
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(GetInput());
	// カメラ
	InitializeCamera();
	// リソース
	InitializeResources();
	// 
	GetSceneData().playerID = kDefaultPlayerId;

	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// ステージ
	titleStage_ = std::make_unique<TitleStage>();
	titleStage_->Initialize( GetEntityManager(),GetAudioManager(), cameraManager_.get());
	titleStage_->SetEffect(effect_.get());

	// UI
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());

	GetAudioManager()->Play("titleWind.mp3", true,0.5f);


	// プレイヤー
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(GetEntityManager(), GetGlobalVariables(), "player", "testCharacter.gltf", 
		false, false, nullptr,Engine::ObjectModelType::kSkinning);
	objectComponent_->GetObject3D()->InitAnimationComponent();
	objectComponent_->GetObject3D()->GetAnimationComponent()->SetAnimation("Rig|Idle_Loop",0.0f);
	objectComponent_->GetWorldTransform().translate_ = kTitlePlayerPosition;

	// オブジェクトコンポーネント追加
	objectComponentShadow_ = std::make_unique<ObjectComponent>();
	// オブジェクトインスタンシング初期化
	objectComponentShadow_->InitializeInstancing(GetEntityManager(), GetGlobalVariables(), "PlayerBase1", "plane.obj", "resources/Texture/smoke/no4.dds",
		false, false, nullptr, Engine::ObjectInstance::TransparencyType::kYes);

	objectComponentShadow_->SetInstancingSRT({ 1.0f,1.0f,1.0f },
		{ Math::DegreesToRadians(kShadowRotationDegrees),0.0f,0.0f }, { 0.0f,kShadowGroundOffset,0.0f });
	objectComponentShadow_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効
}

void TitleScene::Finalize(){
	GetEntityManager()->GetEffectManager()->GetParticleManager()->ClearParticle("dust2");
	GetEntityManager()->GetEffectManager()->GetParticleManager()->ClearParticle("dust3");

}

void TitleScene::Update()
{
	// 共通入力を更新し、ゲームパッドB・Enter・Spaceを決定操作として受け付ける。
	inputCoordinator_->Update(GetTime());
	if (!isStart_ && inputCoordinator_->GetInputSystem()->GetGameInputData().decisionTrigger) {
		titleUI_->Action();
		isStart_ = true;
		objectComponent_->GetObject3D()->GetAnimationComponent()->SetAnimation("SwordRun01", kAnimationBlendSeconds);
	}
	if (isStart_) {
		if (objectComponent_->GetWorldTransform().translate_.z >= titleStage_->GetPlayerCar()->GetBodyWorldPosition().z) {
			objectComponent_->GetWorldTransform().translate_.z = titleStage_->GetPlayerCar()->GetBodyWorldPosition().z;
			objectComponent_->GetObject3D()->GetAnimationComponent()->SetAnimation("Rig|Idle_Loop", kAnimationBlendSeconds);
			objectComponent_->GetWorldTransform().scale_ = { 0.0f,0.0f,0.0f };
			objectComponentShadow_->GetWorldTransform().scale_ = { 0.0f,0.0f,0.0f };
			//
			startTimer_ += GetTime();

			if (startTimer_ >= kSceneTransitionDelaySeconds) {
				GetSceneManager()->ChangeScene("SELECT");
			}

			// 動く
			titleStage_->GetPlayerCar()->Action();
		}
		else {
			objectComponent_->GetWorldTransform().translate_.z += kPlayerApproachSpeedPerFrame;
		}
	}

	// プレイヤー
	objectComponent_->Update();

	objectComponentShadow_->GetWorldTransform().translate_.x = objectComponent_->GetWorldTransform().translate_.x;
	objectComponentShadow_->GetWorldTransform().translate_.z = objectComponent_->GetWorldTransform().translate_.z;
	objectComponentShadow_->GetWorldTransform().translate_.y = kShadowGroundOffset;

	objectComponentShadow_->Update();

	//// 位置
	//weapon_->GetObject3D()->GetWorldTransform().SetParent(Engine::AnimationFunction::GetWorldMatrixOfJoint(
	//	objectComponent_->GetObject3D()->GetModel()->modelData.skeleton, "DEF-hand.R",
	//	objectComponent_->GetWorldTransform().worldMat_));
	//// 武器
	//weapon_->Update();
	// ステージ更新
	titleStage_->Update(GetTime());
	// エフェクト
	effect_->Update(GetTime());
	// UI更新
	titleUI_->Update(GetTime());
	// カメラ管理更新
	cameraManager_->Update();
}

void TitleScene::Draw3D(){}

void TitleScene::Draw2D(){
	// UI更新
	titleUI_->Draw();
}

void TitleScene::InitializeResources()
{
	// オブジェクト3D
	GetEntityManager()->GetObject3dCommon()->SetDefaultCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstanceManager()->SetCamera(cameraManager_->GetCamera());

}

void TitleScene::InitializeCamera()
{
	// タイトルシーン用カメラ
	titleCamera_ = std::make_unique<TitleCamera>();
	titleCamera_->Initialize(nullptr, GetEntityManager(), GetGlobalVariables(), kTitleCameraPosition);


	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(nullptr, GetEntityManager(), GetGlobalVariables());
	cameraManager_->AddCamera({ titleCamera_.get(), true}, "titleCamera");
	
	SetCamera(cameraManager_->GetCamera());
}
