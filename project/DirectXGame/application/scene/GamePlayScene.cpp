#include "GamePlayScene.h"
#include <iostream>
#include <corecrt_math_defines.h>
#include <algorithm>

#include <exception>
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

namespace {
	// デバッグ操作でカメラを切り替える際の補間時間。
	constexpr float kDebugCameraBlendSeconds = 0.3f;
	// リトライ時のシーン遷移時間。
	constexpr float kRetryTransitionSeconds = 0.5f;
	// タイトルへ戻る際のシーン遷移時間。
	constexpr float kTitleTransitionSeconds = 0.25f;
	// ゲーム開始時のプレイヤー座標。
	const Vector3 kPlayerInitialPosition = { 0.0f, 2.0f, -40.0f };
}

#pragma region Initialize
// 初期化
void GamePlayScene::Initialize() {
	GetSceneData().score = 0;
	// Input
	input = GetInput();

	// 選択されたプレイヤー種別を共通基盤へ渡す。
	const Character::PlayerType playerType = GetSceneData().playerID == 1
		? Character::PlayerType::kNormal : Character::PlayerType::kBullet;
	// ゲームプレイで共通利用する入力、カメラ、キャラクター、衝突基盤を一括初期化する。
	gameplaySession_ = std::make_unique<GameplaySession>();
	gameplaySession_->Initialize(input, GetEntityManager(), GetGlobalVariables(), playerType, kPlayerInitialPosition);
	InputSystem* inputSystem = gameplaySession_->GetInputCoordinator()->GetInputSystem();
	CameraManager* cameraManager = gameplaySession_->GetCameraManager();
	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(inputSystem, GetEntityManager(), GetGlobalVariables(), {});
	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(inputSystem, GetEntityManager(), GetGlobalVariables(), {});
	// ステージカメラ
	stageCamera_ = std::make_unique<StageCamera>();
	stageCamera_->Initialize(inputSystem, GetEntityManager(), GetGlobalVariables(), {});
	// ゲームプレイ専用カメラを共通カメラ管理へ追加する。
	cameraManager->AddCamera({ universeCamera_.get(),false }, "universeCamera");
	cameraManager->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");
	cameraManager->AddCamera({ stageCamera_.get(),false }, "stageCamera");

	// ステージイベントマネージャー
	gameFlowController_ = std::make_unique<GameFlowController>();
	gameFlowController_->Initialize(GetSceneManager(), inputSystem, cameraManager, GetGlobalVariables(), gameplaySession_->GetCharacterManager());

	// ステージ
	stage_ = std::make_unique<MainStage>();
	stage_->Initialize(GetEntityManager(), GetAudioManager(), cameraManager);
	
	// UI
	gameUI_ = std::make_unique<GameUI>();
	gameUI_->Initialize(inputSystem, GetEntityManager(), GetGlobalVariables());
	
	poseSystem_ = std::make_unique<PoseSystem>();
	poseSystem_->Initialize(GetSceneManager(), inputSystem, GetEntityManager(), GetGlobalVariables());

	poseUI_ = std::make_unique<PoseUI>();
	poseUI_->Initialize(inputSystem, GetEntityManager(), GetGlobalVariables());


	// カメラ設定
	SetCamera(cameraManager->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager->GetCamera());
	GetEntityManager()->GetObject3dInstanceManager()->SetCamera(cameraManager->GetCamera());
}

// 終了
void GamePlayScene::Finalize() {
	// エンティティ側のインスタンシング描画リソースをシーン終了時に空にする。
	if (GetEntityManager() && GetEntityManager()->GetObject3dInstanceManager()) {
		GetEntityManager()->GetObject3dInstanceManager()->AllClear();
	}
	// 共通基盤内の衝突情報を、関連オブジェクト破棄前に消しておく。
	if (gameplaySession_ && gameplaySession_->GetCollisionRegistrationSystem()) {
		gameplaySession_->GetCollisionRegistrationSystem()->GetCollisionManager()->Clear();
	}

	// カメラ参照を持つUI/進行/ステージを先に破棄する。
	poseUI_.reset();
	poseSystem_.reset();
	gameUI_.reset();
	stage_.reset();
	gameFlowController_.reset();

	// GameplaySession内のCameraManagerと追従カメラを明示終了する。
	if (gameplaySession_) {
		gameplaySession_->Finalize();
		gameplaySession_.reset();
	}

	// GameplaySessionのCameraManagerへ登録していた追加カメラを破棄する。
	stageCamera_.reset();
	fixedCamera_.reset();
	universeCamera_.reset();
	input = nullptr;
}

#pragma endregion 初期化関係


#pragma region UpDate

// ImGui更新
void GamePlayScene::UpdateImGui()
{
#ifdef _DEBUG
	if (input->IsTriggerKey(DIK_P)) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}

	ImGui::Begin("Debug");
	ImGui::InputInt("playerID", &GetSceneData().playerID);

	Vector2 inputPos = input->GetGamePadLeftStick();
	ImGui::InputFloat2("Input", &inputPos.x);
	if (ImGui::Button("lockOn")) {
		gameplaySession_->GetCameraManager()->SetUseCamera("fixedCamera", kDebugCameraBlendSeconds);
	}
	if (ImGui::Button("noLockOn")) {
		gameplaySession_->GetCameraManager()->SetUseCamera("followCamera", kDebugCameraBlendSeconds);
	}


	ImGui::End();
#endif // _DEBUG

	gameUI_->SetImageLeftTopPosAndRatio(GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(), GetDxCommon()->GetPostEffectManager()->GetImageRatio());

}

// 更新処理
void GamePlayScene::Update()
{
	// 共通基盤から、このフレームで使用する各管理クラスを取得する。
	InputCoordinator* inputCoordinator = gameplaySession_->GetInputCoordinator();
	Character::CharacterManager* characterManager = gameplaySession_->GetCharacterManager();
	SpecialPointManager* specialPointManager = gameplaySession_->GetSpecialPointManager();
	BulletManager* bulletManager = gameplaySession_->GetBulletManager();
	HitBox::System* hitBoxSystem = gameplaySession_->GetHitBoxSystem();
	CollisionRegistrationSystem* collisionRegistrationSystem = gameplaySession_->GetCollisionRegistrationSystem();
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
	characterManager->Update(GetTime(),gameFlowController_->IsMove());
	// 必殺技ポイント管理クラス
	specialPointManager->SetTarget(characterManager->GetPlayer());
	specialPointManager->Update(GetTime());
	// 弾マネージャ
	bulletManager->Update();
	// ステージ
	stage_->Update(GetTime());
	// ヒットボックスシステム更新
	hitBoxSystem->Update(GetTime());
	// 当たり判定
	collisionRegistrationSystem->RegisterAllCollisions();
	// Effect更新
	gameplaySession_->GetEffectSystem()->Update(GetTime());
	// ゲーム進行マネージャー更新
	gameFlowController_->Update(GetTime());

	if (characterManager->GetPlayer()) {
		if (characterManager->GetPlayer()->GetAlive()) {
			GetSceneData().playerDie = false;
		}
	}
	else {
		GetSceneData().playerDie = true;
	}
	GetSceneData().score = characterManager->GetScore();
	gameUI_->SetPlayer(characterManager->GetPlayer());
	gameUI_->SetGamePlayData(gameFlowController_->GetGamePlayData());
	gameUI_->Update(GetTime());
	// ポーズシステム更新
	poseSystem_->Update(GetTime());
	// ポーズUI更新
	poseUI_->Update(GetTime());

	// カメラ管理の更新
	gameplaySession_->GetCameraManager()->Update();
}

#pragma endregion //更新関係



// 3D描画
void GamePlayScene::Draw3D(){
	////3Dオブジェクトの描画
	gameplaySession_->GetBulletManager()->Draw();
	gameplaySession_->GetBulletManager()->DrawEffect();
	gameplaySession_->GetEffectSystem()->Draw();

	// ゲーム進行
	gameFlowController_->Draw();
}

// 2D描画
void GamePlayScene::Draw2D(){
	// ゲームUI
	gameUI_->Draw();
	// キャラクター
	gameplaySession_->GetCharacterManager()->Draw2D();
	// 弾マネージャ
	gameplaySession_->GetBulletManager()->Draw2D();
	// ゲーム進行UI
	gameFlowController_->Draw2D();
	// ポーズUI
	poseUI_->Draw();
}

