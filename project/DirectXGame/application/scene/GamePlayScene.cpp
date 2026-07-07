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

#pragma region Initialize
// 初期化
void GamePlayScene::Initialize() {
	GetSceneData().score = 0;
	// Input
	input = GetInput();

	// シーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(input);

	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});
	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});
	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});
	// ステージカメラ
	stageCamera_ = std::make_unique<StageCamera>();
	stageCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});

	// カメラ管理
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	// カメラ追加
	cameraManager_->AddCamera({ followCamera_.get(),true }, "followCamera");
	cameraManager_->AddCamera({ universeCamera_.get(),false }, "universeCamera");
	cameraManager_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");
	cameraManager_->AddCamera({ stageCamera_.get(),false }, "stageCamera");


	GetEntityManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());

	// ヒットボックスシステム初期化
	hitBoxSystem_ = std::make_unique<HitBox::System>();
	hitBoxSystem_->Initialize(GetEntityManager());

	// 弾管理クラス
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(GetEntityManager(), GetGlobalVariables(), nullptr);
	bulletManager_->SetEffect(effect_.get());

	// スペシャルポイント管理クラス
	specalPointManager_ = std::make_unique<SpecalPointManager>();
	specalPointManager_->Initialize(GetEntityManager(), GetGlobalVariables());


	// キャラクター管理 
	characterManager_ = std::make_unique<Character::CharacterManager>();
	characterManager_->Initialize(inputCoordinator_->GetInputSystem(), hitBoxSystem_.get(), GetEntityManager(), GetGlobalVariables(), cameraManager_->GetCamera());
	characterManager_->SetEffect(effect_.get());
	characterManager_->SetFollowCamera(followCamera_.get());
	characterManager_->SetBulletManager(bulletManager_.get());
	characterManager_->SetCameraManager(cameraManager_.get());
	characterManager_->SetSpecialPointManager(specalPointManager_.get());

	// ステージイベントマネージャー
	gameFlowController_ = std::make_unique<GameFlowController>();
	gameFlowController_->Initialize(GetSceneManager(),inputCoordinator_->GetInputSystem(), cameraManager_.get(), GetGlobalVariables(), characterManager_.get());

	// プレイヤー生成
	if (GetSceneData().playerID == 1) {
		characterManager_->CreateCharacter(Character::PlayerType::kNormal, "", { 0,2,-40 });
	}
	else {
		characterManager_->CreateCharacter(Character::PlayerType::kBullet, "", { 0,2,-40 });
	}


	// 追従カメラtarget設定
	followCamera_->SetTarget(&characterManager_->GetPlayer()->GetObjectComponent()->GetWorldTransform());

	// ステージ
	stage_ = std::make_unique<MainStage>();
	stage_->Initialize(GetEntityManager(), cameraManager_.get());
	
	RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(characterManager_->GetPlayer()->GetSpecial());
	sp->SetStage(stage_.get());


	// 衝突登録システム
	collisionRegistrationSystem_ = std::make_unique<CollisionRegistrationSystem>();
	collisionRegistrationSystem_->Initialize(GetGlobalVariables(), GetEntityManager()->Get3DLineCommon(), hitBoxSystem_.get(),
		characterManager_.get(),specalPointManager_.get(),bulletManager_.get());

	// UI
	gameUI = std::make_unique<GameUI>();
	gameUI->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	
	poseSystem_ = std::make_unique<PoseSystem>();
	poseSystem_->Initialize(GetSceneManager(), inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());

	poseUI_ = std::make_unique<PoseUI>();
	poseUI_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());


	// カメラ設定
	SetCamera(cameraManager_->GetCamera());

	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());

	inputCoordinator_->SetOwner(characterManager_->GetPlayer());
}

// 終了
void GamePlayScene::Finalize() {
	GetEntityManager()->GetObject3dInstansManager()->AllClear();
	collisionRegistrationSystem_->GetCollisionManager()->Clear();
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
		cameraManager_->SetUseCamera("fixedCamera", 0.3f);
	}
	if (ImGui::Button("noLockOn")) {
		cameraManager_->SetUseCamera("followCamera", 0.3f);
	}


	ImGui::End();
#endif // _DEBUG

	gameUI->SetImageLeftTopPosAndRatio(GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(), GetDxCommon()->GetPostEffectManager()->GetImageRatio());

}

// 更新処理
void GamePlayScene::Update()
{
	// 入力更新とプレイヤーコマンド実行を一括で行う
	inputCoordinator_->Update(GetTime(), characterManager_->GetPlayer());

	// リトライ
	if (input->IsTriggerKey(DIK_R)) {
		GetSceneManager()->ChangeScene("GAMEPLAY", 0.5f);
	}
	if (input->IsTriggerKey(DIK_T)) {
		GetSceneManager()->ChangeScene("TITLE", 0.25f);
	}
	
	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();
	// キャラクターマネージャー更新
	characterManager_->Update(GetTime(),gameFlowController_->IsMove());
	// 必殺技ポイント管理クラス
	specalPointManager_->SetTarget(characterManager_->GetPlayer());
	specalPointManager_->Update(GetTime());
	// 弾マネージャ
	bulletManager_->Update();
	// ステージ
	stage_->Update(GetTime());
	// ヒットボックスシステム更新
	hitBoxSystem_->Update(GetTime());
	// 当たり判定
	collisionRegistrationSystem_->RegisterAllCollisions();
	// Effect更新
	effect_->Update(GetTime());
	// ゲーム進行マネージャー更新
	gameFlowController_->Update(GetTime());

	if (characterManager_->GetPlayer()) {
		if (characterManager_->GetPlayer()->GetAlive()) {
			GetSceneData().playerDie = false;
		}
	}
	else {
		GetSceneData().playerDie = true;
	}
	GetSceneData().score = characterManager_->GetScore();
	gameUI->SetPlayer(characterManager_->GetPlayer());
	gameUI->SetGamePlayData(gameFlowController_->GetGamePlayData());
	gameUI->Update(GetTime());
	// ポーズシステム更新
	poseSystem_->Update(GetTime());
	// ポーズUI更新
	poseUI_->Update(GetTime());

	// カメラ管理の更新
	cameraManager_->Update();
}

#pragma endregion //更新関係



// 3D描画
void GamePlayScene::Draw3D(){
	////3Dオブジェクトの描画
	bulletManager_->DrawEffect();
	// ゲーム進行
	gameFlowController_->Draw();
}

// 2D描画
void GamePlayScene::Draw2D(){
	// ゲームUI
	gameUI->Draw();
	// キャラクター
	characterManager_->Draw2D();
	// 弾マネージャ
	bulletManager_->Draw2D();
	// ゲーム進行UI
	gameFlowController_->Draw2D();
	// ポーズUI
	poseUI_->Draw();
}

