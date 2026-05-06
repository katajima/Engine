#include "CharacterDebugScene.h"
#include <iostream>
#include <corecrt_math_defines.h>
#include <algorithm>

#include <exception>
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"


void CharacterDebugScene::Initialize()
{
	// Input
	input_ = GetInput();

	// インプットマネージャー初期化
	inputManager_ = std::make_unique<InputManager>();
	inputManager_->Initialize(input_);


	// インプットハンドラー初期化
	inputHander_ = std::make_unique<Character::InputHander>();
	inputHander_->Initialize(input_);


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
	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables(), {});
	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables(), {});

	// カメラ管理
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputSystem_.get(), GetEntityManager(), GetGlobalVariables());
	// カメラ追加
	cameraManager_->AddCamera({ followCamera_.get(),true }, "followCamera");
	cameraManager_->AddCamera({ universeCamera_.get(),false }, "universeCamera");
	cameraManager_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");



	// 弾管理クラス
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(GetEntityManager(),GetGlobalVariables(), nullptr);
	bulletManager_->SetEffect(effect_.get());

	// 弾スポーン
	bulletSpawn_ = std::make_unique<BulletSpawn>();
	bulletSpawn_->Initialize(nullptr, GetEntityManager(), GetGlobalVariables(), nullptr, effect_.get(), bulletManager_.get());

	param.name = "test";
	param.modelName = "AnimatedCube.gltf";
	param.maxLifeTime = 5.0f;
	param.speed = 10.0f;
	param.gravityScale = 0.1f;
	param.moveType = Projectile::ProjectileMoveType::Homing;
	param.enableHoming = true;
	param.homingRange = 50.0f;
	param.homingStrength = 5.0f;

	spawnInfo.position = { 10,2,-40 };
	spawnInfo.direction = { -1,0,0 };
	spawnInfo.scale = { 0.5f,0.5f,0.5f };

	// ヒットボックスシステム初期化
	hitBoxSystem_ = std::make_unique<HitBox::System>();
	hitBoxSystem_->Initialize(GetEntityManager());

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
	if (GetSceneData().playerID == 1) {
		characterManager_->CreateCharacter(Character::PlayerType::kNormal, "", { 0,2,-40 });
	}
	else {
		characterManager_->CreateCharacter(Character::PlayerType::kBullet, "", { 0,2,-40 });
	}

	spawnInfo.target = characterManager_->GetPlayer();

	// 追従カメラtarget設定
	followCamera_->SetTarget(&characterManager_->GetPlayer()->GetObjectComponent()->GetWorldTransform());

	// ステージ
	stage_ = std::make_unique<MainStage>();
	stage_->Initialize(GetEntityManager(), cameraManager_.get());

	RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(characterManager_->GetPlayer()->GetSpecial());
	sp->SetStage(stage_.get());

	// 衝突マネージャの生成
	Vector3 sizeAABB = { 1000,1000,1000 };
	collisionManager_ = std::make_unique<Engine::CollisionManager>();
	collisionManager_->Initialize(GetGlobalVariables(), AABB(-sizeAABB, sizeAABB));
	//collisionManager_->RegisterStatic(stage_->GetStageColliderSystem()->GetColliderComponent());
	//collisionManager_->BuildStaticSceneOctree();

	// カメラ設定
	SetCamera(cameraManager_->GetCamera());

	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());

	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<Engine::EffectComponent>();
	effectComponent_->Init(GetEntityManager(), GetGlobalVariables());


	inputManager_->SetOwner(characterManager_->GetPlayer());



	// ダミー敵生成
	tagNumber = characterManager_->CreateCharacter(Character::EnemyType::kDummy, "dummy", 0, { {1,1,1},{},{} });

	// コンボエディター初期化
	comboEditor_ = std::make_unique<Combo::Editor>();
	comboEditor_->Initialize(GetEntityManager()->Get3DLineCommon(), 
		characterManager_->GetPlayer()->GetAttackController()->GetComboSystem(), 
		GetGlobalVariables(), characterManager_->GetPlayer(),effect_.get());
}

void CharacterDebugScene::Finalize(){
	GetEntityManager()->GetObject3dInstansManager()->AllClear();
	collisionManager_->Clear();
}

void CharacterDebugScene::Update(){
	
	inputSystem_->Update(GetTime());

	// リトライ
	if (input_->IsTriggerKey(DIK_R)) {
		GetSceneManager()->ChangeScene("CHARACTER", 0.5f);
	}
	if (input_->IsTriggerKey(DIK_T)) {
		GetSceneManager()->ChangeScene("TITLE", 0.25f);
	}
	// デバッグモード設定
	characterManager_->GetPlayer()->GetAttackController()->SetIsDebug(comboEditor_->IsActive());

	// コンボエディター更新
	comboEditor_->Update(GetTime());

	// インプットマネージャー更新
	inputManager_->Update(GetTime());

	// コマンド
	iCommand_ = inputHander_->HandleInput();
	if (this->iCommand_) {
		iCommand_->Exec(*characterManager_->GetPlayer());
	}

	interval_ += GetTime();
	if (interval_ >= intervalMax_) {
		interval_ = 0.0f;
		//bulletSpawn_->GenerateProjectile(spawnInfo, param);
	}


	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();


	// キャラクターマネージャー更新
	characterManager_->Update(true);

	// 必殺技ポイント管理クラス
	specalPointManager_->Update(GetTime());



	// カメラ管理の更新
	cameraManager_->Update();
	// 弾マネージャ
	bulletManager_->Update();
	// ステージ
	stage_->Update(GetTime());
	// ヒットボックスシステム更新
	hitBoxSystem_->Update(GetTime());
	// 当たり判定
	CheckAllCollisions();
	// Effect更新
	effect_->Update(GetTime());
}

void CharacterDebugScene::Draw3D(){
	////3Dオブジェクトの描画
	bulletManager_->DrawEffect();
}

void CharacterDebugScene::Draw2D(){
	// ゲームUI
	//gameUI->Draw();
	// キャラクター
	characterManager_->Draw2D();
	// 弾マネージャ
	bulletManager_->Draw2D();
}

// ImGui更新
void CharacterDebugScene::UpdateImGui() {

#ifdef _DEBUG
	if (input_->IsTriggerKey(DIK_P)) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}

	ImGui::Begin("Debug");
	ImGui::InputInt("playerID", &GetSceneData().playerID);

	static const char* EnemyTypeLabels[] = {
				"小型近距離",
				"小型遠距離",
				"中型近距離",
				"ダミー"
	};
	Engine::ImGuiManager::Select("敵種類", EnemyTypeLabels, enemyType);
	ImGui::DragFloat3("敵出現位置", &enemyPos.x, 0.1f);
	if (ImGui::Button("敵出現")) {
		characterManager_->CreateCharacter(enemyType, "enemy", 0, { {1,1,1},{},enemyPos });
	}
	if (ImGui::Button("ダミー位置リセット")) {
		characterManager_->GetEnemy(tagNumber)->GetObjectComponent()->GetWorldTransform().translate_ = {};
	}
	Vector2 inputPos = input_->GetGamePadLeftStick();
	ImGui::InputFloat2("Input", &inputPos.x);
	

	ImGui::End();


#endif // _DEBUG
}

// グローバルバリアブル適応
void CharacterDebugScene::ApplyGlobalVariables() {
}

/// <summary>
/// 衝突判定と応答
/// </summary>
void CharacterDebugScene::CheckAllCollisions() {
	for (auto objects : stage_->GetLoadLevelData()->GetObjects()) {
		if (objects->GetColliderComponent()) {
			collisionManager_->Register(objects->GetColliderComponent());
		}
	}
	// キャラクターセット
	for (auto caracter : characterManager_->GetCharacters()) {
		if (caracter->GetColliderComponent()) {
			if (caracter->GetHP() <= 0) continue;
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
	for (const auto& bullet : bulletManager_->GetProjectiles()) {
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

