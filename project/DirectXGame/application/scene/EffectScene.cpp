#include "EffectScene.h"

void EffectScene::Initialize() {
	// デバッグシーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(GetInput());
	// カメラ管理
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	// カメラ追加
	


	// スカイボックス初期化
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(GetEntityManager(), "resources/Texture/hdr/sky.dds");

	// 空
	sky_ = GetEntityManager()->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {}, cameraManager_->GetCamera());
	sky_->GetWorldTransform().scale_ = skyBoxScale_;
	sky_->SetSkyBox(skyBox.get());
	
	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());
	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<Engine::EffectComponent>();
	effectComponent_->Init(GetEntityManager(), GetGlobalVariables());

	// カメラ設定
	SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstanceManager()->SetCamera(cameraManager_->GetCamera());
}

void EffectScene::Finalize()
{}

void EffectScene::Update()
{
	

	// 時間
	float dt = GetTime();

	// 入力更新とデバッグ対象へのコマンド実行を一括で行う
	inputCoordinator_->Update(dt, nullptr);


	GetEntityManager()->Get3DLineCommon()->GetDebugLineMeshData().AddGrid(1000.0f, 1000.0f, 1.0f, {1,1,1,1});

	// カメラ管理の更新
	cameraManager_->Update();
	// Effect更新
	effect_->Update(dt);
}

void EffectScene::Draw3D() {}

void EffectScene::Draw2D() {}
