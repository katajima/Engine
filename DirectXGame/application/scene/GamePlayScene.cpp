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


	cameraManeger_ = std::make_unique<CameraManeger>();
	cameraManeger_->Initialize(input_,GetEntity3DManager(),GetEntity2DManager(),GetGlobalVariables());


	caracterManager_ = std::make_unique<BaseCharacterManager>();
	caracterManager_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), nullptr);
	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), {}, nullptr);
	caracterManager_->SetFollowCamera(followCamera_.get());
	// 弾管理クラス
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(GetEntity3DManager(), GetEntity2DManager(), nullptr);
	caracterManager_->SetBulletManager(bulletManager_.get());

	// プレイヤー生成
	caracterManager_->CreateCharacter(PlayerType::kNormal, "", { 0,2,-40 });


	// 追従カメラtarget設定
	followCamera_->SetTarget(caracterManager_->GetPlayer());
	// 弾にプレイヤーセット
	bulletManager_->SetPlayer(caracterManager_->GetPlayer());


	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), {}, nullptr);

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
	ParticleManager* particleManager = GetEntity3DManager()->GetEffectManager()->GetParticleManager();


	gameUI->Initialize(GetEntity2DManager());
	gameUI->SetPlayer(caracterManager_->GetPlayer());


	loadData_ = std::make_unique<LoadLevelData>();
	loadData_->Initialize(GetEntity3DManager(), GetDxCommon()->GetModelManager(), nullptr, "gameScene.json");

	for (auto& enemy : loadData_->GetLevelData()->enemys) {
		if(enemy.isEnable)
		caracterManager_->CreateCharacter(EnemyType::kNormal, "", Transform({ 1,1,1 }, enemy.rotation, enemy.position));
	}

	// カメラ追加
	cameraManeger_->AddCamera({ followCamera_.get(),true},"followCamera");
	cameraManeger_->AddCamera({ universeCamera_.get(),false},"universeCamera");
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
	ImGui::End();
#endif // _DEBUG

	if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
		caracterManager_->GetPlayer()->GetSpecial()->SetGauge(100);
	}


	RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(caracterManager_->GetPlayer()->GetSpecial());
	if (sp->IsAction()) {
		cameraManeger_->SetUseCamera("universeCamera",0.0f);

		timer = 0.0f;
		
		cameraScaleT += 0.05f;
		if (cameraScaleT >= 1.0f) {
			cameraScaleT = 1.0f;
		}
		universeCamera_->GetUniqueCamera()->transform_.scale.z = Lerp(minScaleZCamera, 1.0f, cameraScaleT);
	}
	else {
		timer += MyGame::GameTime();
	}

	if (timer >= 1.25f) {
		timer = 0.0f;
		cameraScaleT = 0.0f;
		universeCamera_->GetUniqueCamera()->transform_.scale.z = minScaleZCamera;
		cameraManeger_->SetUseCamera("followCamera", 0.0f);
	}
	
	// カメラ管理の更新
	cameraManeger_->Update();
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

