#include "TutorialScene.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"

namespace {
	// チュートリアル開始時のプレイヤー座標。
	const Vector3 kTutorialPlayerInitialPosition = { 0.0f, 2.0f, -40.0f };
	// リトライ時のシーン遷移時間。
	constexpr float kRetryTransitionSeconds = 0.5f;
	// タイトルへ戻る際のシーン遷移時間。
	constexpr float kTitleTransitionSeconds = 0.25f;
}

void TutorialScene::Initialize() {
	// Input
	input = GetInput();

	// チュートリアルでもゲームプレイ共通基盤を一括初期化する。
	gameplaySession_ = std::make_unique<GameplaySession>();
	gameplaySession_->Initialize(input, GetEntityManager(), GetGlobalVariables(),
		Character::PlayerType::kNormal, kTutorialPlayerInitialPosition);
	InputSystem* inputSystem = gameplaySession_->GetInputCoordinator()->GetInputSystem();
	CameraManager* cameraManager = gameplaySession_->GetCameraManager();
	Character::CharacterManager* characterManager = gameplaySession_->GetCharacterManager();


	tutorialStage_ = std::make_unique<TutorialStage>();
	tutorialStage_->Initialize(GetEntityManager(), GetAudioManager(), cameraManager);

	tutorialSystem_ = std::make_unique<TutorialSystem>();
	tutorialSystem_->Initialize(GetSceneManager(), inputSystem, GetEntityManager(), GetGlobalVariables(), characterManager->GetPlayer());

	tutorialUI_ = std::make_unique<TutorialUI>();
	tutorialUI_->Initialize(inputSystem, GetEntityManager(), GetGlobalVariables());
	
	poseSystem_ = std::make_unique<PoseSystem>();
	poseSystem_->Initialize(GetSceneManager(), inputSystem, GetEntityManager(), GetGlobalVariables());

	poseUI_ = std::make_unique<PoseUI>();
	poseUI_->Initialize(inputSystem, GetEntityManager(), GetGlobalVariables());



	//RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(characterManager_->GetPlayer()->GetSpecial());
	//sp->SetStage(stage_.get());

	// カメラ設定
	SetCamera(cameraManager->GetCamera());
}

void TutorialScene::Finalize() {
	// エンティティ側のインスタンシング描画リソースをシーン終了時に空にする。
	if (GetEntityManager() && GetEntityManager()->GetObject3dInstanceManager()) {
		GetEntityManager()->GetObject3dInstanceManager()->AllClear();
	}
	// 共通基盤内の衝突情報を、関連オブジェクト破棄前に消しておく。
	if (gameplaySession_ && gameplaySession_->GetCollisionRegistrationSystem()) {
		gameplaySession_->GetCollisionRegistrationSystem()->GetCollisionManager()->Clear();
	}

	// カメラ参照を持つUI/システム/ステージを先に破棄する。
	poseUI_.reset();
	poseSystem_.reset();
	tutorialUI_.reset();
	tutorialSystem_.reset();
	tutorialStage_.reset();

	// GameplaySession内のCameraManagerと追従カメラを明示終了する。
	if (gameplaySession_) {
		gameplaySession_->Finalize();
		gameplaySession_.reset();
	}
	input = nullptr;
}

void TutorialScene::Update() {
	// 共通基盤から、このフレームで使用する各管理クラスを取得する。
	InputCoordinator* inputCoordinator = gameplaySession_->GetInputCoordinator();
	Character::CharacterManager* characterManager = gameplaySession_->GetCharacterManager();
	SpecialPointManager* specialPointManager = gameplaySession_->GetSpecialPointManager();
	// 入力更新とプレイヤーコマンド実行を一括で行う
	inputCoordinator->Update(GetTime(), characterManager->GetPlayer());

	// リトライ
	if (input->IsTriggerKey(DIK_R)) {
		GetSceneManager()->ChangeScene("GAMEPLAY", kRetryTransitionSeconds);
	}
	if (input->IsTriggerKey(DIK_T)) {
		GetSceneManager()->ChangeScene("TITLE", kTitleTransitionSeconds);
	}

	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();
	// キャラクターマネージャー更新
	characterManager->Update(GetTime(),true);
	// 必殺技ポイント管理クラス
	specialPointManager->SetTarget(characterManager->GetPlayer());
	specialPointManager->Update(GetTime());
	// 弾マネージャ
	gameplaySession_->GetBulletManager()->Update();
	// ステージ
	tutorialStage_->Update(GetTime());
	// ヒットボックスシステム更新
	gameplaySession_->GetHitBoxSystem()->Update(GetTime());
	// 当たり判定
	gameplaySession_->GetCollisionRegistrationSystem()->RegisterAllCollisions();
	// Effect更新
	gameplaySession_->GetEffectSystem()->Update(GetTime());
	// チュートリアルシステム更新
	tutorialSystem_->Update(GetTime());
	// チュートリアルUI更新
	tutorialUI_->Update(GetTime());
	// ポーズシステム更新
	poseSystem_->Update(GetTime());
	// ポーズUI更新
	poseUI_->Update(GetTime());
	
	
	// カメラ管理の更新
	gameplaySession_->GetCameraManager()->Update();
}

void TutorialScene::Draw3D() {
	////3Dオブジェクトの描画
	gameplaySession_->GetBulletManager()->Draw();
	gameplaySession_->GetBulletManager()->DrawEffect();
}

void TutorialScene::Draw2D() {
	// チュートリアルUI描画
	tutorialUI_->Draw();
	// ポーズUI描画
	poseUI_->Draw();
	// キャラクター
	gameplaySession_->GetCharacterManager()->Draw2D();
	// 弾マネージャ
	gameplaySession_->GetBulletManager()->Draw2D();
	
}
