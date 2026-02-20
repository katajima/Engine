#include "CharacterDebugScene.h"
#include <iostream>
#include <corecrt_math_defines.h>
#include <algorithm>

#include <exception>
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"


void CharacterDebugScene::Initialize()
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
		[this] { return inputManager_->Triggered(InputManager::Action::Skill); },
		std::make_unique<SkillAttackCommand>());

	inputHander_->Bind(
		[this] { return inputManager_->Triggered(InputManager::Action::Move); },
		std::make_unique<MoveCommand>());
	
	// 入力システム初期化
	inputSystem_ = std::make_unique<InputSystem>();
	inputSystem_->Initialize(GetInput());


	// エフェクト
	effect_ = std::make_unique<Effect>();
	effect_->Initialize(GetEntity3DManager(), GetGlobalVariables());

	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(inputSystem_.get(), GetEntity3DManager(), GetGlobalVariables(), {});
	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(inputSystem_.get(), GetEntity3DManager(), GetGlobalVariables(), {});
	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(inputSystem_.get(), GetEntity3DManager(), GetGlobalVariables(), {});

	// カメラ管理
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputSystem_.get(), GetEntity3DManager(), GetGlobalVariables());
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
	characterManager_ = std::make_unique<Character::CharacterManager>();
	characterManager_->Initialize(input_, GetEntity3DManager(), GetEntity2DManager(), GetGlobalVariables(), nullptr);
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


	// レベルデータロード
	loadData_ = std::make_unique<LoadLevelData>();
	loadData_->SetCameraManager(cameraManager_.get());
	loadData_->Initialize(GetEntity3DManager(), GetDxCommon()->GetModelManager(), nullptr, "gameScene.json");


	// 追従カメラtarget設定
	followCamera_->SetTarget(&characterManager_->GetPlayer()->GetObjectComponent()->GetWorldTransform());

	// ステージ
	stage_ = std::make_unique<MainStage>();
	stage_->Initialize(GetDxCommon(), GetEntity3DManager(), GetEntity2DManager(), followCamera_->GetUniqueCamera());

	RangeBombingSpecial* sp = static_cast<RangeBombingSpecial*>(characterManager_->GetPlayer()->GetSpecial());
	sp->SetStage(stage_.get());

	// 衝突マネージャの生成
	Vector3 sizeAABB = { 1000,1000,1000 };
	collisionManager_ = std::make_unique<Engine::CollisionManager>();
	collisionManager_->Initialize(GetGlobalVariables(), AABB(-sizeAABB, sizeAABB));


	collisionManager_->BuildStaticSceneOctree();

	// カメラ設定
	SetCamera(cameraManager_->GetCamera());

	GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntity3DManager()->GetObject3dInstansManager()->SetCamera(cameraManager_->GetCamera());

	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<Engine::EffectComponent>();
	effectComponent_->Init(GetEntity3DManager(), GetGlobalVariables());


	inputManager_->SetOwner(characterManager_->GetPlayer());



	// ダミー敵生成
	characterManager_->CreateCharacter(Character::EnemyType::kDummy, "dummy", 0, { {1,1,1},{},{} });

	// デバッグモード設定
	characterManager_->GetPlayer()->GetAttackController()->SetIsDebug(isComboEditorActive_);
	// コンボエディター初期化
	comboEditor_ = std::make_unique<Combo::Editor>();
	comboEditor_->Initialize(GetEntity3DManager()->Get3DLineCommon(), characterManager_->GetPlayer()->GetAttackController()->GetComboSystem(), GetGlobalVariables(), characterManager_->GetPlayer());
}

void CharacterDebugScene::Finalize(){
	GetEntity3DManager()->GetObject3dInstansManager()->AllClear();
}

void CharacterDebugScene::Update()
{
	Engine::Camera::isShake_ = false;

	// リトライ
	if (input_->IsTriggerKey(DIK_R)) {
		GetSceneManager()->ChangeScene("GAMEPLAY", 0.5f);
	}
	if (input_->IsTriggerKey(DIK_T)) {
		GetSceneManager()->ChangeScene("TITLE", 0.25f);
	}

	// コンボエディター更新
	comboEditor_->Update(GetTime());

	// インプットマネージャー更新
	inputManager_->Update(GetTime());

	// コマンド
	iCommand_ = inputHander_->HandleInput();
	if (this->iCommand_) {
		iCommand_->Exec(*characterManager_->GetPlayer());
	}


	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();


	// キャラクターマネージャー更新
	characterManager_->Update();

	// 必殺技ポイント管理クラス
	specalPointManager_->Update(GetTime());



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
}

// グローバルバリアブル適応
void CharacterDebugScene::ApplyGlobalVariables() {
}

/// <summary>
/// 衝突判定と応答
/// </summary>
void CharacterDebugScene::CheckAllCollisions() {
	for (auto objects : loadData_->GetObjects()) {
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
		auto& hit = caracter->GetAttackController()->GetHitBoxSystem()->GetData();
		for (auto& caracterHitBox : hit) {
			collisionManager_->Register(caracterHitBox.hitBox.get()->GetColliderComponent());
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

