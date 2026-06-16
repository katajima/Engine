#include "TutorialScene.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"

void TutorialScene::Initialize() {
	// Input
	input = GetInput();

	// インプットマネージャー初期化
	inputManager_ = std::make_unique<InputManager>();
	inputManager_->Initialize(input);


	// インプットハンドラー初期化
	inputHander_ = std::make_unique<Character::InputHander>();
	inputHander_->Initialize(input);


	inputManager_->Triggered(InputManager::Action::Jump);

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::Jump); },
		std::make_unique<Character::JampCommand>());

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::LightAttack); },
		std::make_unique<Character::AttackCommand>());

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::HeavyAttack); },
		std::make_unique<Character::HeavyAttackCommand>());
	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::Skill); },
		std::make_unique<Character::SkillAttackCommand>());
	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::Move); },
		std::make_unique<Character::MoveCommand>());

	// 入力システム初期化
	inputSystem_ = std::make_unique<InputSystem>();
	inputSystem_->Initialize(GetInput());

	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());

	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables(), {});

	// カメラ管理
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());
	// カメラ追加
	cameraManager_->AddCamera({ followCamera_.get(),true }, "followCamera");


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
	characterManager_->Initialize(inputSystem_.get(), hitBoxSystem_.get(), GetEntityManager(), GetGlobalVariables(), cameraManager_->GetCamera());
	characterManager_->SetEffect(effect_.get());
	characterManager_->SetFollowCamera(followCamera_.get());
	characterManager_->SetBulletManager(bulletManager_.get());
	characterManager_->SetCameraManager(cameraManager_.get());
	characterManager_->SetSpecialPointManager(specalPointManager_.get());


	// プレイヤー生成
	characterManager_->CreateCharacter(Character::PlayerType::kNormal, "", { 0,2,-40 });

	// 追従カメラtarget設定
	followCamera_->SetTarget(&characterManager_->GetPlayer()->GetObjectComponent()->GetWorldTransform());


	tutorialStage_ = std::make_unique<TutorialStage>();
	tutorialStage_->Initialize(GetEntityManager(), cameraManager_.get());

	tutorialSystem_ = std::make_unique<TutorialSystem>();
	tutorialSystem_->Initialize(GetSceneManager(), inputSystem_.get(), GetEntityManager(), GetGlobalVariables(), characterManager_->GetPlayer());

	tutorialUI_ = std::make_unique<TutorialUI>();
	tutorialUI_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());
	
	poseSystem_ = std::make_unique<PoseSystem>();
	poseSystem_->Initialize(GetSceneManager(), inputSystem_.get(), GetEntityManager(), GetGlobalVariables());

	poseUI_ = std::make_unique<PoseUI>();
	poseUI_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());



	//RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(characterManager_->GetPlayer()->GetSpecial());
	//sp->SetStage(stage_.get());

	// 衝突マネージャの生成
	Vector3 sizeAABB = { 300,25,300 };
	collisionManager_ = std::make_unique<Engine::CollisionManager>();
	collisionManager_->Initialize(GetGlobalVariables(), AABB(-sizeAABB, sizeAABB));


	

	// カメラ設定
	SetCamera(cameraManager_->GetCamera());

	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());

	inputManager_->SetOwner(characterManager_->GetPlayer());
}

void TutorialScene::Finalize() {
	GetEntityManager()->GetObject3dInstansManager()->AllClear();
	collisionManager_->Clear();
}

void TutorialScene::Update() {
	inputManager_->SetOwner(characterManager_->GetPlayer());
	// 入力システム更新
	inputSystem_->Update(GetTime());

	// リトライ
	if (input->IsTriggerKey(DIK_R)) {
		GetSceneManager()->ChangeScene("GAMEPLAY", 0.5f);
	}
	if (input->IsTriggerKey(DIK_T)) {
		GetSceneManager()->ChangeScene("TITLE", 0.25f);
	}

	// インプットマネージャー更新
	inputManager_->Update(GetTime());

	// コマンド
	iCommand_ = inputHander_->HandleInput();
	if (this->iCommand_ && characterManager_->GetPlayer()) {
		iCommand_->Exec(*characterManager_->GetPlayer());
	}


	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();
	// キャラクターマネージャー更新
	characterManager_->Update(GetTime(),true);
	// 必殺技ポイント管理クラス
	specalPointManager_->SetTarget(characterManager_->GetPlayer());
	specalPointManager_->Update(GetTime());
	// 弾マネージャ
	bulletManager_->Update();
	// ステージ
	tutorialStage_->Update(GetTime());
	// ヒットボックスシステム更新
	hitBoxSystem_->Update(GetTime());
	// 当たり判定
	CheckAllCollisions();
	// Effect更新
	effect_->Update(GetTime());
	// チュートリアルシステム更新
	tutorialSystem_->Update(GetTime());
	// チュートリアルUI更新
	tutorialUI_->Update(GetTime());
	// ポーズシステム更新
	poseSystem_->Update(GetTime());
	// ポーズUI更新
	poseUI_->Update(GetTime());
	
	
	// カメラ管理の更新
	cameraManager_->Update();
}

void TutorialScene::Draw3D() {
	////3Dオブジェクトの描画
	bulletManager_->DrawEffect();
}

void TutorialScene::Draw2D() {
	// チュートリアルUI描画
	tutorialUI_->Draw();
	// ポーズUI描画
	poseUI_->Draw();
	// キャラクター
	characterManager_->Draw2D();
	// 弾マネージャ
	bulletManager_->Draw2D();
	
}

// ImGui更新
void TutorialScene::UpdateImGui() {
	// ImGuiの更新ロジック

}

// グローバルバリアブル適応
void TutorialScene::ApplyGlobalVariables() {
	// グローバルバリアブルの適応ロジック

}

/// <summary>
/// 衝突判定と応答
/// </summary>
void TutorialScene::CheckAllCollisions() {
	// 衝突判定と応答のロジック
	// キャラクターセット
	for (auto caracter : characterManager_->GetCharacters()) {
		if (caracter->GetColliderComponent()) {
			if (caracter->GetAlive() && caracter->GetCurrentMainState() != Character::CharacterMainState::Die)
				collisionManager_->Register(caracter->GetColliderComponent());

		}
	}

	// ヒットボックス
	for (auto& hitBoxData : hitBoxSystem_->GetHitBoxData()) {
		collisionManager_->Register(hitBoxData.hitBox.get()->GetColliderComponent());
	}
	for (auto& hitBoxData : hitBoxSystem_->GetLifeTimeHitBoxData()) {
		collisionManager_->Register(hitBoxData.hitBox.get()->GetColliderComponent());
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

	// 描画
	collisionManager_->DrawLine(GetEntityManager()->Get3DLineCommon());
	// 判定チェック
	collisionManager_->CheckAll();
	// 動的コライダー削除
	collisionManager_->ClearDynamic();
}
