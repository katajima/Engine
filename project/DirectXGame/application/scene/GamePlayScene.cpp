#include "GamePlayScene.h"
#include <iostream>
#include <corecrt_math_defines.h>
#include <algorithm>


#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"

float GamePlayScene::nowTime = 0.0f;
#pragma region Initialize

// 初期化
void GamePlayScene::Initialize()
{
	// Input
	input_ = GetInput();

	inputHander_ = std::make_unique<InputHander>();
	inputHander_->SetInput(input_);
	inputHander_->AssignMoveCommandPad();
	inputHander_->AssignJampCommandPad();
	inputHander_->AssignAttackCommandPad();

	//
	effect_ = std::make_unique<Effect>();
	effect_->Initialize(GetEntity3DManager(), GetGlobalVariables());

	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(input_, GetEntity3DManager(), GetGlobalVariables(), {});
	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(input_, GetEntity3DManager(), GetGlobalVariables(), {});
	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(input_, GetEntity3DManager(), GetGlobalVariables(), {});

	// カメラ管理
	cameraManeger_ = std::make_unique<CameraManeger>();
	cameraManeger_->Initialize(input_, GetEntity3DManager(), GetGlobalVariables());
	// カメラ追加
	cameraManeger_->AddCamera({ followCamera_.get(),true }, "followCamera");
	cameraManeger_->AddCamera({ universeCamera_.get(),false }, "universeCamera");
	cameraManeger_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");



	// 弾管理クラス
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), nullptr);
	bulletManager_->SetEffect(effect_.get());

	// キャラクター管理 
	caracterManager_ = std::make_unique<BaseCharacterManager>();
	caracterManager_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), nullptr);
	caracterManager_->SetEffect(effect_.get());
	caracterManager_->SetFollowCamera(followCamera_.get());
	caracterManager_->SetBulletManager(bulletManager_.get());


	// ステージイベントマネージャー
	stageEventManager_ = std::make_unique<StageEventManager>();
	stageEventManager_->Initialize(GetEntity3DManager(), GetGlobalVariables(), caracterManager_.get());

	// プレイヤー生成
	if (GetSceneData().playerID == 1) {
		caracterManager_->CreateCharacter(PlayerType::kNormal, "", { 0,2,-40 });
	}
	else {
		caracterManager_->CreateCharacter(PlayerType::kBullet, "", { 0,2,-40 });
	}

	// 弾にプレイヤーセット
	bulletManager_->SetPlayer(caracterManager_->GetPlayer());


	// レベルデータロード
	loadData_ = std::make_unique<LoadLevelData>();
	loadData_->SetCameraManager(cameraManeger_.get());
	loadData_->Initialize(GetEntity3DManager(), GetDxCommon()->GetModelManager(), nullptr, "gameScene.json");


	// 追従カメラtarget設定
	followCamera_->SetTarget(&caracterManager_->GetPlayer()->GetObjectComponent()->GetWorldTransform());

	// ステージ
	stage_ = std::make_unique<Stage>();
	stage_->Initialize(GetDxCommon(), GetEntity3DManager(), GetEntity2DManager(), followCamera_->GetUniqueCamera());
	
	RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(caracterManager_->GetPlayer()->GetSpecial());
	sp->SetStage(stage_.get());

	// 衝突マネージャの生成
	Vector3 sizeAABB = { 1000,1000,1000 };
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize(GetGlobalVariables(), AABB(-sizeAABB, sizeAABB));


	collisionManager_->BuildStaticSceneOctree();


	// UI
	gameUI = std::make_unique<GameUI>();
	gameUI->Initialize(GetInput(), GetEntity2DManager(), GetGlobalVariables());
	gameUI->SetPlayer(caracterManager_->GetPlayer());

	sprite = std::make_unique<UICount>();
	sprite->SetUseNameSprite(false);
	sprite->SetInstance(2);
	sprite->Init(GetEntity2DManager(), "fps");
	sprite->SetInput(input_);
	sprite->SetPos({ 32,48 });
	sprite->SetMaxSize({ 64 * 2 / 3, 96 * 2 / 3 }, { 20.0f,0.0f });
	sprite->SetTextuerSize({ 64,96 });
	sprite->SetCountMax(999);


	SetCamera(cameraManeger_->GetCamera());

	GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManeger_->GetCamera());

	effectComponent_ = std::make_unique<EffectComponent>();
	effectComponent_->Init(GetEntity3DManager(), GetGlobalVariables());

}

// 調整項目
void GamePlayScene::ApplyGlobalVariables()
{

}

void GamePlayScene::CheckAllCollisions()
{
	// 衝突マネージャのリセット
	collisionManager_->ClearDynamic();
	//collisionManager_->Clear();

	for (auto objects : loadData_->GetObjects()) {
		if (objects->GetColliderComponent()) {
			collisionManager_->Register(objects->GetColliderComponent());
			//collisionManager_->RegisterStatic(objects->GetColliderComponent());
		}
	}
	// キャラクターセット
	for (auto caracter : caracterManager_->GetCharacters()) {
		if (caracter->GetColliderComponent()) {
			if (caracter->GetHP() <= 0) continue;
			collisionManager_->Register(caracter->GetColliderComponent());
		}
	}

	if (GetSceneData().playerID == 1) {
		collisionManager_->Register(caracterManager_->GetPlayer()->GetWeapon()->GetColliderComponent());
	}
	// 弾のコライダー追加
	for (const auto& bullet : bulletManager_->GetBullets()) {
		if (bullet->GetColliderComponent()) {
			collisionManager_->Register(bullet->GetColliderComponent());
		}
	}


	collisionManager_->CheckAll();
	collisionManager_->ClearDynamic();
	//collisionManager_->Clear();
}

#pragma endregion 初期化関係


#pragma region UpDate

// ImGui更新
void GamePlayScene::UpdateImGui()
{
#ifdef _DEBUG

	gameUI->SetImageLeftTopPosAndRatio(GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(), GetDxCommon()->GetPostEffectManager()->GetImageRatio());

#endif // _DEBUG


	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
	nowTime += deltaTime;
	if (deltaTime > 0) {
		fps = 1.0f / deltaTime;
	}
	lastTime = currentTime;

	sprite->SetCount(fps);

	sprite->Update(MyGame::GameTime());
}

// 更新処理
void GamePlayScene::Update()
{
	Camera::isShake_ = false;


	if (input_->IsTriggerKey(DIK_R)) {
		GetSceneManager()->ChangeScene("GAMEPLAY", 0.5f);
	}
	if (input_->IsTriggerKey(DIK_RETURN)) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}

	iCommand_ = inputHander_->HandleInput();
	if (this->iCommand_) {
		iCommand_->Exec(*caracterManager_->GetPlayer());
	}


	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();

	// キャラクターマネージャー更新
	caracterManager_->Update();

	// ステージイベントマネージャー更新
	stageEventManager_->Update();


	if (stageEventManager_->IsEndEvent()) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}

#ifdef _DEBUG
	if (input_->IsTriggerKey(DIK_P)) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}

	ImGui::Begin("Debug");
	ImGui::DragFloat3("enePos", &enemyPosition.x, 0.1f);
	ImGui::InputInt("playerID", &GetSceneData().playerID);
	if (ImGui::Button("lockOn")) {
		cameraManeger_->SetUseCamera("fixedCamera", 0.3f);
	}
	if (ImGui::Button("noLockOn")) {
		cameraManeger_->SetUseCamera("followCamera", 0.3f);
	}


	ImGui::End();


#endif // _DEBUG
	if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
		//	caracterManager_->GetPlayer()->GetSpecial()->SetGauge(100);
	}


	// スペシャル
	if (caracterManager_->GetPlayer()->GetSpecial()->IsAction()) {
		cameraManeger_->SetUseCamera("universeCamera", 0.0f);
	}

	gameUI->Update();
	// カメラ管理の更新
	cameraManeger_->Update();
	// 弾マネージャ
	bulletManager_->Update();
	// ステージ
	stage_->Update();
	// 当たり判定
	CheckAllCollisions();
	// レベルデータアップデート
	loadData_->Update();
	// Effect更新
	effect_->Update();
}

#pragma endregion //更新関係
// 終了
void GamePlayScene::Finalize()
{

}

// 3D描画
void GamePlayScene::Draw3D()
{
	////3Dオブジェクトの描画
	bulletManager_->DrawEffect();
}

// 2D描画
void GamePlayScene::Draw2D()
{
	// ゲームUI
	gameUI->Draw();
	//
	sprite->Draw();
	// キャラクター
	caracterManager_->Draw2D();
	// 弾マネージャ
	bulletManager_->Draw2D();
}

