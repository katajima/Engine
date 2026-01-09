#include "GamePlayScene.h"
#include <iostream>
#include <corecrt_math_defines.h>
#include <algorithm>

#include <exception>
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"

#pragma region Initialize

// 初期化
void GamePlayScene::Initialize()
{
	// Input
	input_ = GetInput();

	// インプットマネージャー初期化
	inputManager_ = std::make_unique<InputManager>();
	inputManager_->Initialize(input_);
	

	// インプットハンドラー初期化
	inputHander_ = std::make_unique<InputHander>();
	inputHander_->Initialize(input_);


	inputManager_->Triggered(InputManager::Action::Jump);

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::Jump); },
		std::make_unique<JampCommand>());

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::LightAttack); },
		std::make_unique<AttackCommand>());

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::HeavyAttack); },
		std::make_unique<HeavyAttackCommand>());

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::Move); },
		std::make_unique<MoveCommand>());


	// エフェクト
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
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(input_, GetEntity3DManager(), GetGlobalVariables());
	// カメラ追加
	cameraManager_->AddCamera({ followCamera_.get(),true }, "followCamera");
	cameraManager_->AddCamera({ universeCamera_.get(),false }, "universeCamera");
	cameraManager_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");



	// 弾管理クラス
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), nullptr);
	bulletManager_->SetEffect(effect_.get());

	// スペシャルポイント管理クラス
	specalPointManager_ = std::make_unique<SpecalPointManager>();
	specalPointManager_->Initialize(GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables());


	// キャラクター管理 
	caracterManager_ = std::make_unique<BaseCharacterManager>();
	caracterManager_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), nullptr);
	caracterManager_->SetEffect(effect_.get());
	caracterManager_->SetFollowCamera(followCamera_.get());
	caracterManager_->SetBulletManager(bulletManager_.get());
	caracterManager_->SetCameraManager(cameraManager_.get());
	caracterManager_->SetSpecialPointManager(specalPointManager_.get());

	// ステージイベントマネージャー
	gameFlowController_ = std::make_unique<GameFlowController>();
	gameFlowController_->Initialize(GetSceneManager(), GetGlobalVariables(), caracterManager_.get());

	// プレイヤー生成
	if (GetSceneData().playerID == 1) {
		caracterManager_->CreateCharacter(PlayerType::kNormal, "", { 0,2,-40 });
	}
	else {
		caracterManager_->CreateCharacter(PlayerType::kBullet, "", { 0,2,-40 });
	}

	
	// レベルデータロード
	loadData_ = std::make_unique<LoadLevelData>();
	loadData_->SetCameraManager(cameraManager_.get());
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
	collisionManager_ = std::make_unique<Engine::CollisionManager>();
	collisionManager_->Initialize(GetGlobalVariables(), AABB(-sizeAABB, sizeAABB));


	collisionManager_->BuildStaticSceneOctree();


	// UI
	gameUI = std::make_unique<GameUI>();
	gameUI->Initialize(GetInput(), GetEntity2DManager(), GetGlobalVariables());
	gameUI->SetPlayer(caracterManager_->GetPlayer());

	// FPS表示用スプライト
	sprite = std::make_unique<Engine::UICount>();
	sprite->SetUseNameSprite(false);		// 使わない
	sprite->SetInstance(2);					// 行数設定
	sprite->Init(GetEntity2DManager(), "fps");	// 初期化
	sprite->SetInput(input_);					// 入力設定
	sprite->SetPos({ 32,48 });					// 位置設定
	sprite->SetMaxSize({ 64 * 2 / 3, 96 * 2 / 3 }, { 20.0f,0.0f });	// 最大値サイズ設定
	sprite->SetTextuerSize({ 64,96 });	// テクスチャサイズ設定
	sprite->SetCountMax(999);			// カウント量設定

	// カメラ設定
	SetCamera(cameraManager_->GetCamera());

	GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());

	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<Engine::EffectComponent>();
	effectComponent_->Init(GetEntity3DManager(), GetGlobalVariables());



	inputManager_->SetOwner(caracterManager_->GetPlayer());
}

// 調整項目
void GamePlayScene::ApplyGlobalVariables()
{

}

void GamePlayScene::CheckAllCollisions()
{
	// 衝突マネージャのリセット
	collisionManager_->ClearDynamic();
	
	for (auto objects : loadData_->GetObjects()) {
		if (objects->GetColliderComponent()) {
			collisionManager_->Register(objects->GetColliderComponent());
		}
	}
	// キャラクターセット
	for (auto caracter : caracterManager_->GetCharacters()) {
		if (caracter->GetColliderComponent()) {
			if (caracter->GetHP() <= 0) continue;
			collisionManager_->Register(caracter->GetColliderComponent());
		}
	}

	// IDが1なら
	if (GetSceneData().playerID == 1) {
		for (auto& caracter : caracterManager_->GetPlayer()->GetAttackController()->GetHitBoxSystem()->GetData()) {
			collisionManager_->Register(caracter.hitBox.get()->GetColliderComponent());
		}
	}
	// 弾のコライダー追加
	for (const auto& bullet : bulletManager_->GetBullets()) {
		if (bullet->GetColliderComponent()) {
			collisionManager_->Register(bullet->GetColliderComponent());
		}
	}

	// SPポイントのコライダー追加
	for (const auto& point : specalPointManager_->GetSpecalPoints()) {
		if (point->GetColliderComponent()) {
			collisionManager_->Register(point->GetColliderComponent());
		}
	}


	collisionManager_->CheckAll();
	collisionManager_->ClearDynamic();
	
}

#pragma endregion 初期化関係


#pragma region UpDate

// ImGui更新
void GamePlayScene::UpdateImGui()
{
#ifdef _DEBUG
	if (input_->IsTriggerKey(DIK_P)) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}

	ImGui::Begin("Debug");
	ImGui::DragFloat3("enePos", &enemyPosition.x, 0.1f);
	ImGui::InputInt("playerID", &GetSceneData().playerID);

	Vector2 inputPos = input_->GetGamePadLeftStick();
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


	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
	nowTime += deltaTime;
	if (deltaTime > 0) {
		fps = 1.0f / deltaTime;
	}
	lastTime = currentTime;

	// fpsカウント設定
	sprite->SetCount(fps);

	// fps用のスプライト更新
	//sprite->Update(Engine::MyGame::GameTime());
}

// 更新処理
void GamePlayScene::Update()
{
	Engine::Camera::isShake_ = false;

	// リトライ
	if (input_->IsTriggerKey(DIK_R)) {
		GetSceneManager()->ChangeScene("GAMEPLAY", 0.5f);
	}
	if (input_->IsTriggerKey(DIK_T)) {
		GetSceneManager()->ChangeScene("TITLE", 0.25f);
	}
	
	// インプットマネージャー更新
	inputManager_->Update(Engine::MyGame::GameTime());

	// コマンド
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

	// 必殺技ポイント管理クラス
	specalPointManager_->Update(Engine::MyGame::GameTime());

	

	// カメラ管理の更新
	cameraManager_->Update();
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
	// ゲーム進行マネージャー更新
	gameFlowController_->Update(Engine::MyGame::GameTime());

	gameUI->SetGamePlayData(gameFlowController_->GetGamePlayData());
	gameUI->Update();

}

#pragma endregion //更新関係

// 終了
void GamePlayScene::Finalize(){}

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
	//
	sprite->Draw();
	// キャラクター
	caracterManager_->Draw2D();
	// 弾マネージャ
	bulletManager_->Draw2D();
}

