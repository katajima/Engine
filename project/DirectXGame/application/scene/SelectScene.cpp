#include "SelectScene.h"


void SelectScene::Initialize() {
	input = GetInput();
	// セレクトカメラ生成
	selectCamera_ = std::make_unique<SelectCamera>();
	selectCamera_->Initialize(nullptr, GetEntity3DManager(), GetGlobalVariables(), { 1.8f,0.7f,3.3f });
	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(nullptr, GetEntity3DManager(), GetGlobalVariables());
	cameraManager_->AddCamera({ selectCamera_.get() ,true }, "selectCamera");

	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(cameraManager_->GetCamera());
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntity3DManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());


	// エフェクト
	effect_ = std::make_unique<Effect>();
	effect_->Initialize(GetEntity3DManager(), GetGlobalVariables());

	// ステージ生成
	selectStage_ = std::make_unique<SelectStage>();
	selectStage_->Initialize(GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), GetCamara());
	selectStage_->SetEffect(effect_.get());
	selectStage_->GetPlayerCar()->GetStateMachine()->ChangeState(CarMainState::SelectMove);
	// UI生成
	selectUI_ = std::make_unique<SelectUI>();
	selectUI_->Initialize(nullptr, GetEntity2DManager(), GetGlobalVariables());
}


// 終了
void SelectScene::Finalize() {}

// 毎フレーム更新
void SelectScene::Update() {

	if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_B)) {
		GetSceneManager()->ChangeScene("GAMEPLAY");
	}

	selectStage_->SetRatio(GetWinApp()->GetCalculateWindowToFullscreenScaleRatioWidth());
	// ステージ更新
	selectStage_->Update(GetTime());
	// UI更新
	selectUI_->Update(GetTime());
	// カメラ更新
	cameraManager_->Update();
}

// 描画
void SelectScene::Draw3D() {
	// ステージ描画
	selectStage_->Draw();
}

void SelectScene::Draw2D() {
	// UI描画
	selectUI_->Draw();
}