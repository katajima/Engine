#include "NormalPlayer.h"

#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include "assert.h"


void NormalPlayer::Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	entity3DManager_ = entity3DManager;	// エンティティ
	entity2DManager_ = entity2DManager;	// エンティティ
	globalVariables_ = globalVariables;	// 保存項目
	camera_ = camera;					// カメラ
	input_ = input;						// 入力
	
	
	
	Engine::ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager_, globalVariables_, "PlayerBase", "origin.gltf", true, true,this, Engine::ObjectModelType::kSkinning);

	// 保存項目追加
	CreateGroup("Player");

	objectComponent_->SetSRT({1,1,1}, {}, position);				//　SRT設定
	objectComponent_->GetObject3D()->InitAnimationComponent();				// アニメーションコンポーネント初期化
	objectComponent_->GetObject3D()->SetIsUpdateColliderComponent(false);		// コライダーコンポーネント内で更新するか

	// キャラクターのパラメータコンポーネントを生成
	characterParameterComponent_ = std::make_unique<CharacterParameterComponent>();


	// HP設定
	Parameters().HP.Initiaize(200, 0, 200, 0);
	Parameters().stamina.Initiaize(100, 0, 50, 0);
	Parameters().speed = 40.0f;// 移動速度設定
	Parameters().jampPower = 100.0f;
	
	
	// 移動コンポーネント初期化
	moveComponent_ = std::make_unique<MovementComponent>();
	moveComponent_->Initialize(globalVariables_, MovementComponent::ControlType::Manual,"");
	moveComponent_->SetMoveType(MoveSystem::MoveType::ACCELERATE);
	moveComponent_->SetIsStickToSpeed(true);
	moveComponent_->SetControlType(MovementComponent::ControlType::Manual);

	moveComponent_->SetMaxJumpCount(2);
	moveComponent_->SetCamera(followCamera_->GetUniqueCamera());
	
	// 保存項目初期化
	InitializeBaseAddItem();

	// SphereColliderを追加
	auto sphere = std::make_unique<Engine::SphereCollider>();
	sphere->tag = CollisionTag::Player;
	sphere->layer = CollisionLayer::Player;
	sphere->collisionMask = 0xFFFFFFFF;
	sphere->radius = 2.0f; // 半径を適宜設定
	sphere->Enable();
	// コライダ追加
	GetColliderComponent()->AddCollider(std::move(sphere));
	// コンポーネント衝突インターフェース設定
	GetColliderComponent()->SetHitReceiver(this);

	// コライダ位置用トランスフォーム初期化
	worldCollider_.Initialize();
	worldCollider_.parent_ = &GetObjectComponent()->GetWorldTransform();
	worldCollider_.translate_.y = 3.0f;

	// 衝突時のコールバック登録
	GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		uint32_t otherId = otherComponent->GetUniqueId();

		// 敵との衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Enemy,self,other);

		// 壁との衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Wall,self,other);
		};


	// 応答システム初期化
	responseSystem_ = std::make_unique<ResponseSystem>();
	responseSystem_->Initialize(GetCharacterParameterComponent(), objectComponent_.get());
	responseSystem_->GetHitResponse()->SetOwner(&objectComponent_->GetWorldTransform());



	// 弾出現
	bulletSpawn_ = std::make_unique<BulletSpawn>();
	bulletSpawn_->Initialize(this,entity3DManager,entity2DManager,globalVariables,camera,effect_);

	// スペシャル攻撃
	special_ = std::make_unique<RangeBombingSpecial>();
	special_->Initialize(entity3DManager, entity2DManager, camera_);
	special_->SetOwner(this);
	special_->SetParent(&GetObjectComponent()->GetWorldTransform());
	special_->SetInput(input);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
	rengeSp->SetRadius(50);
	rengeSp->SetReticleParent(&GetObjectComponent()->GetWorldTransform());
	rengeSp->Set(followCamera_, bulletSpawn_.get());

	// 攻撃系初期化
	InitAttack();

	// UI
	ui_ = std::make_unique<PlayerUI>();
	ui_->Initialize(input_, entity2DManager_, globalVariables_);
	ui_->SetCharacterParameter(GetCharacterParameterComponent());
	

	// キャラクター行動ステート初期化
	InitStateMachine();
}

void NormalPlayer::InitAttack(){
	// 武器
	weapon_ = std::make_unique<PlayerWeapon>();
	weapon_->SetCharacter(this);
	weapon_->Initialize(input_, entity3DManager_, nullptr, globalVariables_, {}, camera_);
	weapon_->GetObject3D()->GetWorldTransform().rotate_ = { Math::DegreesToRadians(-90),0.0f,0.0f };


	// 戦闘
	attackController_ = std::make_unique<AttackController>();
	attackController_->Initialize(entity3DManager_,globalVariables_, GetCharacterParameterComponent(), this);
	attackController_->GetHitCounter().SetHitTimer(2.0f);
	
	ComboSystem* combo = attackController_->GetComboSystem();

	//
	combo->ApplyGlobalComboData("AttackComboData1", data1_);
	combo->ApplyGlobalComboData("AttackComboData2", data2_);
	combo->ApplyGlobalComboData("AttackComboData3", data3_);
	combo->ApplyGlobalComboData("AttackComboData(Jump)", data4_);
	combo->ApplyGlobalComboData("AttackComboData5", data5_);
	combo->ApplyGlobalComboData("AttackComboData6", data6_);
	combo->ApplyGlobalComboData("AttackComboData7", data7_);
	combo->ApplyGlobalComboData("AttackComboData8", data8_);

	
	// 
	ReloadComboData();

	// インプットハンドラー
	attackInputHandler_ = std::make_unique<AttackInputHandler>();
	attackInputHandler_->AssignAttack();
}

void NormalPlayer::RequestAttack(AttackInput input)
{
	const auto s = stateMachine_->GetCurrentMainState();
	const bool canStart = (s == CharacterMainState::Move) ||
		(s == CharacterMainState::Idle) ||
		(s == CharacterMainState::Jump);
	const bool isAttack = (s == CharacterMainState::Attack);

	auto* ac = GetAttackController();
	ac->SetIsAttack(true);

	if (isAttack)
	{
		ac->GetComboSystem()->InputCombo(input);
		return;
	}

	if (canStart)
	{
		stateMachine_->ChangeState(CharacterMainState::Attack);

		// 開始コンボ名の決定を「入力種類×状況」でまとめる
		if (s == CharacterMainState::Jump)
		{
			ac->GetComboSystem()->StartCombo("JumpAttack");
		}
		else
		{
			// ここをテーブル化するとスキル追加が楽
			switch (input)
			{
			case AttackInput::Light: ac->GetComboSystem()->StartCombo("Attack4"); break;
			case AttackInput::Heavy: ac->GetComboSystem()->StartCombo("Attack1"); break;
			case AttackInput::Skill: ac->GetComboSystem()->StartCombo("Skill1"); break;
			default: break;
			}
		}
	}
}

// ステート初期化and追加
void NormalPlayer::InitStateMachine() {
	// ステートマシーン初期化
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->RegisterState(CharacterMainState::Idle, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateIdle>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateMove>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateAttack>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Special, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateSpecial>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Skill, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateSkill>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Defense, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateDefense>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Fainting, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateFainting>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Jump, [](BaseCharacter* p) {
		return std::make_unique<PlayerStateJump>(p);
		});
	stateMachine_->Init(this, CharacterMainState::Move);
}

// 更新
void NormalPlayer::Update()
{
	UpdateBaseGetValue(); //保存機能 基本値の更新
	ApplyGlobalVariables();

	// HPが0以下なら死亡
	if (GetHP() <= 0) {
		objectComponent_->GetObjectStateFlags().isAlive = false;
	}


#ifdef _DEBUG
	ImGui::Begin("Debug");
	ImGui::InputFloat("HP", &HP());
	if (ImGui::Button("SP")) {
		special_->SetGauge(100);
	}
	ImGui::End();


	ImGui::Begin("Comdo");
	if (ImGui::Button("Relord")) {
		ReloadComboData();
	}
	ImGui::End();

	// クリエイティブモード切替
	if (input_->IsTriggerKey(DIK_C)) {
		if (!isCreativeMode) {
			isCreativeMode = true;

		}
		else {
			isCreativeMode = false;
		}
	}
#endif // _DEBUG


	if (special_->GetPhese() == 1) {
		attackController_->IsStopHitTimer(true);
	}
	else {
		attackController_->IsStopHitTimer(false);
	}

	if (moveComponent_->GetIsLanding() && 
		stateMachine_->GetCurrentMainState() != CharacterMainState::Jump &&
		input_->GetGamePadLeftStick().Length() == 0) {
		moveComponent_->Velocity() = {};
	}
	
	// 必殺技
	special_->Update();
	
	// 攻撃制御更新
	attackController_->Update(GetTime());

	// 応答システム
	responseSystem_->Update(GetTime());
	
	// ワールドトランスフォーム更新
	GetObjectComponent()->GetWorldTransform().Update();
	
	// コライダのワールドトランスフォーム更新
	worldCollider_.Update();

	// コライダーコンポーネント更新
	GetObjectComponent()->GetColliderComponent()->UpdateAll(worldCollider_);

	// キャラクターパラメーター更新
	characterParameterComponent_->Update();

	// 移動コンポーネント更新
	moveComponent_->Update(GetTime(), GetObjectComponent()->GetWorldTransform(),
		*GetObjectComponent()->GetRigidBodyComponent(), GetInput());

	// ステート
	stateMachine_->Update();

	//武器更新
	weapon_->GetObject3D()->GetWorldTransform().SetParent(Engine::AnimationFunction::GetWorldMatrixOfJoint(GetObjectComponent()->GetObject3D()->model->modelData.skeleton, "rightHand", GetObjectComponent()->GetWorldTransform().worldMat_));
	weapon_->Update();

	// UI更新
	ui_->SetImageLeftTopPosAndRatio(entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(), entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageRatio());
	ui_->Update();
}

#pragma region Draw

void NormalPlayer::DrawEffect()
{
}

void NormalPlayer::Draw2D()
{
	// UI表示
	ui_->SetIsTextmax(special_->GetIsSpecial());
	

	ui_->SetIsTextRB(special_->GetIsSpecial() || special_->IsAction());

	ui_->SetSpecialGaugeSize(static_cast<float>(special_->GetGauge()));

	ui_->Draw();
}


#pragma endregion //描画関係

#pragma region Move

void NormalPlayer::Move()
{
	// 重力
	if (stateMachine_->GetCurrentMainState() != CharacterMainState::Jump) {
		Velocity() = { 0,0,0 };
	}


	bool is = stateMachine_->GetCurrentMainState() == CharacterMainState::Move;
	bool is2 = stateMachine_->GetCurrentMainState() == CharacterMainState::Idle;

	// 移動処理
	if (is || is2) {
		moveComponent_->SetCanMove(true);
	}
}

void NormalPlayer::Jump()
{
	bool is = stateMachine_->GetCurrentMainState() == CharacterMainState::Move;
	bool is2 = stateMachine_->GetCurrentMainState() == CharacterMainState::Idle;


	// 生きていてステートの状態が移動状態ならジャンプステートへ移動
	if (GetAlive() && (is || is2) &&
		moveComponent_->GetIsJump() && moveComponent_->GetIsLanding()) {

		stateMachine_->ChangeState(CharacterMainState::Jump);
		GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimation("JumpStrat1", 0.01f);
	}

}

#pragma endregion //移動関係

#pragma region MyRegion

void NormalPlayer::ApplyGlobalVariables(){
	ComboSystem* combo = attackController_->GetComboSystem();

	combo->SetGlobalComboData("AttackComboData1", data1_);
	combo->SetGlobalComboData("AttackComboData2", data2_);
	combo->SetGlobalComboData("AttackComboData3", data3_);
	combo->SetGlobalComboData("AttackComboData(Jump)", data4_);
	combo->SetGlobalComboData("AttackComboData5", data5_);
	combo->SetGlobalComboData("AttackComboData6", data6_);
	combo->SetGlobalComboData("AttackComboData7", data7_);
	combo->SetGlobalComboData("AttackComboData8", data8_);

}


void NormalPlayer::ReloadComboData()
{
	// コンボノードクリア
	GetAttackController()->GetComboSystem()->ClearNode();
	HitBoxSystem* hitBoxSystem = GetAttackController()->GetHitBoxSystem();
	ComboSystem* comboSystem = GetAttackController()->GetComboSystem();
	// ヒットボックスデータ
	HitBoxCollData hitData1 = {}, hitData2 = {}, hitData3 = {}, hitData4 = {}, hitData5 = {};
	
	// ヒットボックスデータ作成
	GlobalHitBoxdata hitBoxdata  = { provisionalData_.collider1Pos ,provisionalData_.obbColliderSize };
	GlobalHitBoxdata hitBoxdata2 = { provisionalData_.collider2Pos ,provisionalData_.obbCollider2Size };
	GlobalHitBoxdata hitBoxdata3 = { { 0,0,3 } ,{ 10,10,10 } };
	GlobalHitBoxdata hitBoxdata4 = { { 0,0,0 },{},20.0f };
	GlobalHitBoxdata hitBoxdata5 = { provisionalData_.collider3Pos ,  provisionalData_.obbCollider3Size };
	
	hitBoxSystem->CreateHitBoxCollData("obbColl1",HitBoxShape::kOBB,HitBoxUseType::kPlayer, hitBoxdata, hitData1);
	hitBoxSystem->CreateHitBoxCollData("obbColl2", HitBoxShape::kOBB, HitBoxUseType::kPlayer,hitBoxdata2, hitData2);
	hitBoxSystem->CreateHitBoxCollData("obb", HitBoxShape::kOBB, HitBoxUseType::kPlayer,hitBoxdata3, hitData3);
	hitBoxSystem->CreateHitBoxCollData("obb2", HitBoxShape::kSphere, HitBoxUseType::kPlayer,hitBoxdata4, hitData4);
	hitBoxSystem->CreateHitBoxCollData("obbColl1_1", HitBoxShape::kOBB, HitBoxUseType::kPlayer,hitBoxdata5, hitData5);
	

	// データ
	// コンボ１のデータ送る
	comboSystem->CreateCombo("Attack1",{{ hitData3 ,data1_ }}, &objectComponent_->GetObject3D()->GetWorldTransform());
	// コンボ２のデータ送る
	comboSystem->CreateCombo("Attack2", { { hitData1 ,data2_ },{ hitData2 ,data2_ } }, &weapon_->GetObject3D()->GetWorldTransform());
	// コンボ３のデータ送る
	comboSystem->CreateCombo("Attack3", { { hitData5 ,data3_ },{ hitData1 ,data3_ } }, &weapon_->GetObject3D()->GetWorldTransform());
	// ジャンプコンボのデータ送る
	comboSystem->CreateCombo("JumpAttack", { { hitData4 ,data4_ } }, &objectComponent_->GetObject3D()->GetWorldTransform(),
		{ ComboCondition::EndConditionType::kOnGround },
		{ComboHitBox::HitBoxSpawnType::kOnGround, HitBoxSystem::Type::kParentIndependent,{ 0,0,5.5f } });
	// コンボ4のデータ送る
	comboSystem->CreateCombo("Attack4", { { hitData3 ,data5_ }}, &objectComponent_->GetObject3D()->GetWorldTransform());
	// コンボ5のデータ送る
	comboSystem->CreateCombo("Attack5", { { hitData3 ,data6_ } }, &objectComponent_->GetObject3D()->GetWorldTransform());
	// コンボ6のデータ送る
	comboSystem->CreateCombo("Attack6", { { hitData3 ,data7_ } }, &objectComponent_->GetObject3D()->GetWorldTransform());
	// コンボ7のデータ送る
	comboSystem->CreateCombo("Attack7", { { hitData3 ,data8_ } }, &objectComponent_->GetObject3D()->GetWorldTransform());



	// コンボ連結設定
	comboSystem->ConnectCombo("Attack4", AttackInput::Light, "Attack5"); // コンボ連結
	comboSystem->ConnectCombo("Attack5", AttackInput::Light, "Attack6"); // コンボ連結
	comboSystem->ConnectCombo("Attack6", AttackInput::Light, "Attack7"); // コンボ連結
	comboSystem->ConnectCombo("Attack7", AttackInput::Light, "Attack1"); // コンボ連結

	comboSystem->ConnectCombo("Attack1", AttackInput::Light, "Attack2"); // コンボ連結
	comboSystem->ConnectCombo("Attack2", AttackInput::Light, "Attack3"); // コンボ連結
	comboSystem->ConnectCombo("JumpAttack", AttackInput::Light, "Attack2"); // コンボ連結	
}

#pragma endregion // そのほか
