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


void CharacterDebugScene::Initialize() {
	// 入力関係初期化
	InitializeInput();
	// エフェクト関係初期化
	InitializeEffect();
	// カメラ関係初期化
	InitializeCamera();
	// 基本的な初期化
	InitializeBase();
	// キャラクター関係初期化
	InitializeCharacter();
	// デバッグ関係初期化
	InitializeDebug();
	// デバッグ対象設定
	SetDebugTarget(tagNumber);

	// 衝突登録システム
	collisionRegistrationSystem_ = std::make_unique<CollisionRegistrationSystem>();
	collisionRegistrationSystem_->Initialize(GetGlobalVariables(), GetEntityManager()->Get3DLineCommon(), hitBoxSystem_.get(),
		characterManager_.get(), specialPointManager_.get(), bulletManager_.get());


	// カメラ設定
	SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
	GetEntityManager()->GetObject3dInstanceManager()->SetCamera(cameraManager_->GetCamera());
}

void CharacterDebugScene::Finalize(){
	GetEntityManager()->GetObject3dInstanceManager()->AllClear();
	collisionRegistrationSystem_->GetCollisionManager()->Clear();
}

void CharacterDebugScene::Update(){
	// 時間
	float dt = GetTime();
	// 調整項目
	ApplyGlobalVariables();
	// デバッグ関係更新
	UpdateDebug(dt);
	// 入力更新
	UpdateInput(dt);
	// キャラクター更新
	UpdateCharacter(dt);
	// 基本的な更新
	UpdateBase(dt);
}

void CharacterDebugScene::Draw3D(){
	////3Dオブジェクトの描画
	bulletManager_->Draw();
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

// 入力関係初期化
void CharacterDebugScene::InitializeInput(){
	// Input
	input_ = GetInput();
	// デバッグシーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(input_);
}
// キャラクター関係初期化
void CharacterDebugScene::InitializeCharacter(){
	// キャラクター管理 
	characterManager_ = std::make_unique<Character::CharacterManager>();
	characterManager_->Initialize(inputCoordinator_->GetInputSystem(), hitBoxSystem_.get(), GetEntityManager(), GetGlobalVariables(), cameraManager_->GetCamera());
	characterManager_->SetEffect(effect_.get());
	characterManager_->SetFollowCamera(followCamera_.get());
	characterManager_->SetBulletManager(bulletManager_.get());
	characterManager_->SetCameraManager(cameraManager_.get());
	characterManager_->SetSpecialPointManager(specialPointManager_.get());

	tagNumber = characterManager_->CreateCharacter(Character::PlayerType::kNormal, "", { 0,2,-40 });
	// ダミー敵生成
	characterManager_->CreateCharacter(Character::EnemyType::kDummy, "dummy", 0, { {1,1,1},{},{} });
	// スモール敵生成
	//tagNumber = characterManager_->CreateCharacter(Character::EnemyType::kSmallMelee, "smallMelee", 0, { {1,1,1},{},{0,0,10} });
}
// カメラ関係初期化
void CharacterDebugScene::InitializeCamera(){
	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});
	// 宇宙カメラ
	universeCamera_ = std::make_unique<UniverseCamera>();
	universeCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});
	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});

	// カメラ管理
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	// カメラ追加
	cameraManager_->AddCamera({ followCamera_.get(),true }, "followCamera");
	cameraManager_->AddCamera({ universeCamera_.get(),false }, "universeCamera");
	cameraManager_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");
}
// エフェクト関係初期化
void CharacterDebugScene::InitializeEffect(){
	// エフェクト
	effect_ = std::make_unique<EffectSystem>();
	effect_->Initialize(GetEntityManager(), GetGlobalVariables());
	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<Engine::EffectComponent>();
	effectComponent_->Init(GetEntityManager(), GetGlobalVariables());
}
// 基本的な初期化
void CharacterDebugScene::InitializeBase(){
	// 弾管理クラス
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(GetEntityManager(), GetGlobalVariables(), nullptr);
	bulletManager_->SetEffect(effect_.get());

	// ヒットボックスシステム初期化
	hitBoxSystem_ = std::make_unique<HitBox::System>();
	hitBoxSystem_->Initialize(GetEntityManager());

	// スペシャルポイント管理クラス
	specialPointManager_ = std::make_unique<SpecialPointManager>();
	specialPointManager_->Initialize(GetEntityManager(), GetGlobalVariables());

	// ステージ
	stage_ = std::make_unique<MainStage>();
	stage_->Initialize(GetEntityManager(), GetAudioManager(), cameraManager_.get());
}
// デバック関係初期化
void CharacterDebugScene::InitializeDebug(){
	// コンボエディター初期化
	comboEditor_ = std::make_unique<Combo::Editor>();
	comboEditor_->Initialize(GetEntityManager()->Get3DLineCommon(),
		GetGlobalVariables(), characterManager_.get(), nullptr, effect_.get());

	// 弾デバッグ初期化
	projectileDebug_ = std::make_unique<Projectile::ProjectileDebug>();
	projectileDebug_->Initialize(GetEntityManager(), GetGlobalVariables(), effect_.get(),
		bulletManager_.get(), nullptr);
}


// 入力関係更新
void CharacterDebugScene::UpdateInput(float dt){
	// コンボエディター操作中は、マウスやキーボード入力でデバッグ対象が動かないようにする。
	inputCoordinator_->SetPlayerInputBlocked(comboEditor_ && comboEditor_->IsInputCaptured());
	// 入力更新とデバッグ対象へのコマンド実行を一括で行う
	inputCoordinator_->Update(dt, debugTarget);

}
// キャラクター関係更新
void CharacterDebugScene::UpdateCharacter(float dt){
	// デバッグモード設定
	debugTarget->GetAttackController()->SetIsDebug(comboEditor_->IsActive());
	// キャラクターマネージャー更新
	characterManager_->Update(dt, true);
	// デバッグ対象ののHP設定
	debugTarget->GetBasicParameters()->HP.value = hp;
}
// 基本的な更新
void CharacterDebugScene::UpdateBase(float dt){

	// 必殺技ポイント管理クラス
	specialPointManager_->Update(dt);
	// カメラ管理の更新
	cameraManager_->Update();
	// 弾マネージャ
	bulletManager_->Update();
	// ステージ
	stage_->Update(dt);
	// ヒットボックスシステム更新
	hitBoxSystem_->Update(dt);
	// 衝突判定と応答
	collisionRegistrationSystem_->RegisterAllCollisions();
	// Effect更新
	effect_->Update(dt);
}
// デバッグ関係更新
void CharacterDebugScene::UpdateDebug(float dt){
	// ImGuiの更新
	UpdateImGui();
	// コンボエディター更新
	comboEditor_->Update(dt);
	// 弾デバッグ更新
	projectileDebug_->Update();
}
// ImGui更新
void CharacterDebugScene::UpdateImGui() {

#ifdef _DEBUG
	ImGui::Begin("Debug");
	ImGui::InputInt("playerID", &GetSceneData().playerID);
	Vector2 inputPos = input_->GetGamePadLeftStick();
	ImGui::InputFloat2("Input", &inputPos.x);
	
	ImGui::DragFloat3("shakeData.offset", &shakeData.offset.x, 0.01f);
	ImGui::DragFloat("shakeData.duration", &shakeData.duration, 0.01f);
	if (ImGui::Button("シェイク")) {
		
		cameraManager_->GetBaseCamera()->GetCameraController()->GetShake()->Request(shakeData);
	}
	ImGui::DragFloat("zoomData.targetDistance", &zoomData.targetDistance, 0.01f);
	ImGui::DragFloat("zoomData.duration", &zoomData.duration, 0.01f);
	ImGui::DragFloat("zoomData.zoomSpeed", &zoomData.zoomSpeed, 0.01f);
	if (ImGui::Button("ズーム")) {
		cameraManager_->GetBaseCamera()->GetCameraController()->GetZoom()->Request(zoomData);
	}
	ImGui::End();
#endif // _DEBUG
}

void CharacterDebugScene::SetDebugTarget(uint32_t tag) {
	// デバック対象設定
	debugTarget = characterManager_->GetCharacter(tag);
	// 追従カメラtarget設定
	followCamera_->SetTarget(&debugTarget->GetObjectComponent()->GetWorldTransform());
	// 入力管理所有者設定
	inputCoordinator_->SetOwner(debugTarget);
	// コンボエディタのターゲット設定
	comboEditor_->SetOwner(debugTarget);
	// ターゲット設定
	projectileDebug_->SetTarget(debugTarget);
	// 入力システム設定
	debugTarget->SetInputSystem(inputCoordinator_->GetInputSystem());
	// カメラ管理取得
	debugTarget->SetCamera(cameraManager_->GetCamera());
}

