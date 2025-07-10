#include "GamePlayScene.h"
#include <iostream>//用いるヘッダファイルが変わります。
#include <corecrt_math_defines.h>
#include <algorithm>


#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "DirectXGame/application/base/Special/BulletSpecial.h"

#pragma region Initialize

// 初期化
void GamePlayScene::Initialize()
{
	debugTimer_.StartTimer();
	debugTimerAll_.StartTimer();
	input_ = GetInput();


	// カメラ
	InitializeCamera();
	debugTimer_.EndTimer();
	debugTimer_.LogTimeSec("GamePlaySceneInit ", "camera");
	debugTimer_.StartTimer();

	// オブジェクト3D
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());


	caracterManager_ = std::make_unique<BaseCharacterManager>();
	caracterManager_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), camera.get());
	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), {}, nullptr);
	caracterManager_->SetFollowCamera(followCamera_.get());
	// 弾管理クラス
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(GetEntity3DManager(), GetEntity2DManager(), camera.get());
	caracterManager_->SetBulletManager(bulletManager_.get());

	// プレイヤー生成
	caracterManager_->CreateCharacter(PlayerType::kNormal, "", { 0,2,-40 });


	// 追従カメラtarget設定
	followCamera_->SetTarget(caracterManager_->GetPlayer());
	// 弾にプレイヤーセット
	bulletManager_->SetPlayer(caracterManager_->GetPlayer());


	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(GetEntity3DManager()->GetCameraCommon());

	// ステージ
	stage_ = std::make_unique<Stage>();
	stage_->Initialize(GetDxCommon(), GetEntity3DManager(), GetEntity2DManager(), followCamera_->GetUniqueCamera());
	RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(caracterManager_->GetPlayer()->GetSpecial());
	sp->SetStage(stage_.get());




	// 衝突マネージャの生成
	Vector3 sizeAABB = { 1000,1000,1000 };
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize(GetGlobalVariables(), AABB(-sizeAABB, sizeAABB));
	// オブジェクト3D
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());
	ParticleManager* particleManager = GetEntity3DManager()->GetEffectManager()->GetParticleManager();


	gameUI->Initialize(GetEntity2DManager());
	gameUI->SetPlayer(caracterManager_->GetPlayer());


	loadData_ = std::make_unique<LoadLevelData>();
	loadData_->Initialize(GetEntity3DManager(), GetDxCommon()->GetModelManager(), camera.get(), "gameScene.json");

	for (auto& enemy : loadData_->GetLevelData()->enemys) {
		if(enemy.isEnable)
		caracterManager_->CreateCharacter(EnemyType::kNormal, "", Transform({ 1,1,1 }, enemy.rotation, enemy.position));
	}
}


// カメラ初期化
void GamePlayScene::InitializeCamera()
{
	camera = std::make_unique <Camera>();

	camera->Initialize(GetEntity3DManager()->GetCameraCommon());
	//camera = Camera::GetInstance();
	camera->transform_.rotate = { 0.36f,0,0 };
	camera->transform_.translate = { 5,32.5f,-59.2f };

	flag = true;
	SetCamera(camera.get());
}

// 調整項目
void GamePlayScene::ApplyGlobalVariables()
{

}

void GamePlayScene::CheckAllCollisions()
{
	// 衝突マネージャのリセット
	collisionManager_->Clear();

	// プレイヤーコライダーセット
	//collisionManager_->Register(player_->GetColliderComponent());

	for (auto objects : loadData_->GetObjects()) {
		if (objects->GetIsColliderComponent()) {
			collisionManager_->Register(objects->GetColliderComponent());
		}
	}

	// キャラクターセット
	for (auto caracter : caracterManager_->GetCharacters()) {
		if (caracter->GetHP() <= 0) continue;
		collisionManager_->Register(caracter->GetColliderComponent());
	}
	// 武器コライダコンセット
	//if (caracterManager_->GetPlayer()->GetBasicBehavior() == BasicBehavior::kAttack) {

	collisionManager_->Register(caracterManager_->GetPlayer()->GetWeapon()->GetColliderComponent());
	//}
	// 弾のコライダー追加
	for (const auto& bullet : bulletManager_->GetBullets()) {
		collisionManager_->Register(bullet->GetColliderComponent());
	}

	collisionManager_->CheckAll();
	collisionManager_->Clear();
}
#pragma endregion 初期化関係


#pragma region UpDate

// ImGui更新
void GamePlayScene::UpdateImGui()
{
	if (input_->IsTriggerKey(DIK_P)) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}
	if (caracterManager_->GetCharacterCount(CharacterType::Enemy) <= 0 || !caracterManager_->GetPlayer()->GetAlive()) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}

#ifdef _DEBUG
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("Translate", &camera->transform_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &camera->transform_.rotate.x, 0.01f);
		ImGui::Checkbox("flag", &flag);
		if (ImGui::Button("cameraPos")) {
			camera->transform_.translate = { 0,20,-175 };
			camera->transform_.rotate = { 0,0,0 };
		}
		if (ImGui::Button("cameraPos2")) {
			camera->transform_.translate = { -30,10,-140 };
			camera->transform_.rotate = { 0,0,0 };
		}
		if (ImGui::Button("cameraPos3")) {
			camera->transform_.translate = { 0,500,0 };
			camera->transform_.rotate = { DegreesToRadians(90),0,0 };
		}
		if (ImGui::Button("cameraPos4")) {
			camera->transform_.translate = { 0,60,-220 };
			camera->transform_.rotate = { DegreesToRadians(10),0,0 };
		}


	}

	if (ImGui::TreeNode("Test")) {
		ImGui::Text("Camera1");
		ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
		//ImGui::TreePop();
		ImGui::Text("Camera2");
		ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
		ImGui::TreePop();
	}


	ImGui::End();

#endif
}

// 更新処理
void GamePlayScene::Update()
{
	Camera::isShake_ = false;

	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();

	int countIndex = 0;
	for (auto& enemy : loadData_->GetLevelData()->enemys) {
		if (enemy.isEnable)
		if (loadData_->GetLevelData()->counts[countIndex] < enemy.count) {
			enemy.crrentTimer += MyGame::GameTime();
			if (enemy.crrentTimer >= enemy.timer) {
				caracterManager_->CreateCharacter(EnemyType::kNormal, "", Transform({ 1,1,1 }, enemy.rotation, enemy.position));
				loadData_->GetLevelData()->counts[countIndex]++;
				enemy.crrentTimer = 0;
			}
		}
		countIndex++;
	}

	if (behaviorRequest_) {
		// ふるまいを変更する
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kPhase1: // フェーズ１
			BehaviorPhase1Initialize();
			break;
		case Behavior::kPhase2: // フェーズ２
			BehaviorPhase2Initialize();
			break;
		}
		// ふるまいリクエストリセット
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_) {
	case Behavior::kPhase1: // フェーズ１
		BehaviorPhase1Update();
		break;
	case Behavior::kPhase2: // フェーズ２
		BehaviorPhase2Update();
		break;
	}

#ifdef _DEBUG
	ImGui::Begin("Debug");
	ImGui::DragFloat3("enePos", &enemyPosition.x, 0.1f);
	if (ImGui::Button("ADDEnemy")) {
		caracterManager_->CreateCharacter(EnemyType::kNormal, "", Transform({ 1,1,1 }, {}, enemyPosition));
	}
	ImGui::Checkbox("isUniverseCamera", &isUniverseCamera);
	ImGui::End();
#endif // _DEBUG

	if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
		caracterManager_->GetPlayer()->GetSpecial()->SetGauge(100);
	}


	RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(caracterManager_->GetPlayer()->GetSpecial());
	if (sp->IsAction()) {
		timer = 0.0f;
		isUniverseCamera = true;

		cameraScaleT += 0.05f;
		if (cameraScaleT >= 1.0f) {
			cameraScaleT = 1.0f;
		}
		universeCamera_->GetViewProjection().transform_.scale.z = Lerp(minScaleZCamera, 1.0f, cameraScaleT);
	}
	else {
		timer += MyGame::GameTime();
	}

	if (timer >= 1.25f) {
		timer = 0.0f;
		cameraScaleT = 0.0f;
		universeCamera_->GetViewProjection().transform_.scale.z = minScaleZCamera;
		isUniverseCamera = false;
	}
	/// レールカメラ
	// カメラの回転を設定
	if (flag) {
		universeCamera_->Update();
		followCamera_->Update();

		if (isUniverseCamera) {
			camera->viewMatrix_ = universeCamera_->GetViewProjection().viewMatrix_;
			camera->projectionMatrix_ = universeCamera_->GetViewProjection().projectionMatrix_;
			GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(&universeCamera_->GetViewProjection());
			GetEntity3DManager()->Get3DLineCommon()->SetDefaltCamera(&universeCamera_->GetViewProjection());
		}
		else {
			camera->viewMatrix_ = followCamera_->GetUniqueCamera()->viewMatrix_;
			camera->projectionMatrix_ = followCamera_->GetUniqueCamera()->projectionMatrix_;
			GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(followCamera_->GetUniqueCamera());
			GetEntity3DManager()->Get3DLineCommon()->SetDefaltCamera(followCamera_->GetUniqueCamera());
		}
		GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());
	}
	else {
		GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());
		GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());
		GetEntity3DManager()->Get3DLineCommon()->SetDefaltCamera(camera.get());
		camera->UpdateMatrix();
	}


	// レベルデータアップデート
	loadData_->Update();
	// 弾マネージャ
	bulletManager_->Update();
	// ステージ
	stage_->Update();
	// 当たり判定
	CheckAllCollisions();
}

#pragma endregion //更新関係

#pragma region BehaviorPhase

void GamePlayScene::BehaviorPhase1Initialize()
{
}

void GamePlayScene::BehaviorPhase1Update()
{

	caracterManager_->Update();

}

void GamePlayScene::BehaviorPhase2Initialize()
{
}

void GamePlayScene::BehaviorPhase2Update()
{
}
#pragma endregion // フェーズ

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
	// キャラクター
	caracterManager_->Draw2D();
	// 弾マネージャ
	bulletManager_->Draw2D();
}

