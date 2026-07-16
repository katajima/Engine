#include "ResultScene.h"


namespace {
	// リザルト画面用カメラの初期座標。
	const Vector3 kResultCameraPosition = { 0.0f, 3.7f, -20.0f };
	// ゲームオーバー表示の画面座標。
	const Vector2 kGameOverUiPosition = { 640.0f, 200.0f };
	// ゲームオーバー表示の中央アンカー。
	const Vector2 kGameOverUiAnchor = { 0.5f, 0.5f };
	// ゲームオーバー表示の描画サイズ。
	const Vector2 kGameOverUiSize = { 256.0f, 64.0f };
}

// 初期化
void ResultScene::Initialize() {

	// シーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(GetInput());

	// リザルト用カメラ初期化
	resultCamera_ = std::make_unique<ResultCamera>();
	resultCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), kResultCameraPosition);
	// カメラ管理クラス初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	cameraManager_->AddCamera({ resultCamera_.get(), true}, "resultCamera");

	GetEntityManager()->GetObject3dCommon()->SetDefaultCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstanceManager()->SetCamera(cameraManager_->GetCamera());
	SetCamera(cameraManager_->GetCamera());

	// エフェクト初期化
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// ステージ初期化
	resultStage_ = std::make_unique<ResultStage>();
	resultStage_->Initialize(GetEntityManager(), GetAudioManager(), cameraManager_.get());
	resultStage_->SetEffect(effect_.get());

	// リザルトUI初期化
	resultUI_ = std::make_unique<ResultUI>();
	resultUI_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());

	// リザルトスコアUI初期化
	resultScoreUI_ = std::make_unique<ResultScoreUI>();
	resultScoreUI_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());

	overUI_ = std::make_unique<Engine::Sprite>();
	overUI_->Initialize(GetEntityManager()->GetSpriteCommon(), "resources/Texture/text/over.dds");
	overUI_->SetPosition(kGameOverUiPosition);
	overUI_->SetAnchorPoint(kGameOverUiAnchor);
	//overUI_->SetTextureSize({256,64});
	overUI_->SetSize(kGameOverUiSize);

	// リザルトシステム初期化
	resultSystem_ = std::make_unique<ResultSystem>();
	resultSystem_->Initialize(GetSceneManager(),inputCoordinator_->GetInputSystem(),
		GetEntityManager(), GetGlobalVariables());

};

// 終了
void ResultScene::Finalize() {
	// リザルト画面のUI/ステージはカメラ参照を持つため先に破棄する。
	overUI_.reset();
	resultSystem_.reset();
	resultScoreUI_.reset();
	resultUI_.reset();
	resultStage_.reset();
	effect_.reset();

	// CameraManagerが描画系へ渡した参照を外してからカメラ本体を破棄する。
	if (cameraManager_) {
		cameraManager_->Finalize();
		cameraManager_.reset();
	}
	resultCamera_.reset();
	inputCoordinator_.reset();
};

// 毎フレーム更新
void ResultScene::Update() {

	// 入力システム更新
	inputCoordinator_->Update(GetTime());

	// リザルトシステム更新
	resultSystem_->Update(GetTime(), GetSceneData().score);

	// ステージ更新
	resultStage_->Update(GetTime());

	// UI更新
	resultUI_->SetSelectedIndex(resultSystem_->GetSelectedIndex());
	resultUI_->Update(GetTime());

	// UI更新
	resultScoreUI_->SetScore(resultSystem_->GetDisplayScore());
	resultScoreUI_->Update(GetTime());

	overUI_->Update();

	effect_->Update(GetTime());

	// カメラ更新
	cameraManager_->Update();
};

// 描画3d
void ResultScene::Draw3D() {
};

// 描画2d
void ResultScene::Draw2D() {
	// メニュー入力待ちになってから選択UIを表示する。
	if (resultSystem_->IsMenuActive()) {
		resultUI_->Draw();
	}

	resultScoreUI_->Draw();

	if (GetSceneData().playerDie) {
		overUI_->Draw();
	}
};
