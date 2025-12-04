#include "NormalPlayer.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "assert.h"


void NormalPlayer::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	entity3DManager_ = entity3DManager;	// エンティティ
	entity2DManager_ = entity2DManager;	// エンティティ
	globalVariables_ = globalVariables;	// 保存項目
	camera_ = camera;					// カメラ
	input_ = input;						// 入力
	
	
	
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager_, globalVariables_, "PlayerBase", "origin.gltf", true, true,this, ObjectModelType::kSkinning);

	// 保存項目追加
	CreateGroup("Player");

	objectComponent_->SetSRT({1,1,1}, {}, position);				//　SRT設定
	objectComponent_->GetObject3D()->InitAnimationComponent();				// アニメーションコンポーネント初期化
	objectComponent_->GetObject3D()->SetIsUpdateColliderComponent(false);		// コライダーコンポーネント内で更新するか


	// HP設定
	Parameters().HP.Initiaize(100, 0, 100, 0);
	Parameters().stamina.Initiaize(100, 0, 50, 0);
	HP() = 100; // 初期HP設定
	Parameters().speed = 40.0f;// 移動速度設定
	Parameters().jampPower = 100.0f;
	
	// 移動コンポーネント初期化
	InitMoveComponent();
	moveComponent_->SetMoveType(MoveSystem::MoveType::ACCELERATE);
	moveComponent_->SetIsStickToSpeed(true);
	moveComponent_->SetControlType(MovementComponent::ControlType::Manual);

	moveComponent_->SetMaxJumpCount(2);
	moveComponent_->SetCamera(followCamera_->GetUniqueCamera());
	
	// 保存項目初期化
	InitializeBaseAddItem();

	// SphereColliderを追加
	auto sphere = std::make_unique<SphereCollider>();
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
	GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		uint32_t otherId = otherComponent->GetUniqueId();

		// 敵との衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Enemy,self,other);

		// 壁との衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Wall,self,other);

		
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		if (!enemy) return;
		if (enemy->GetCharacterStateMachine()->GetCurrentMainState() == CharacterMainState::Attack) {
			float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

			if (objectComponent_->GetContactRecord().CheckHistory(otherId, nowTime, 1.0f)) {
				return; // クールタイム中のため無視
			}

			objectComponent_->GetContactRecord().AddHistory(otherId, nowTime);

			AddDamage(DamageCalculator::ComputeDamage(*enemy->GetAttackController()->GetCombatStat(), *GetAttackController()->GetCombatStat(), 1.0f));
			followCamera_->GetUniqueCamera()->SetShake(0.25f, { 0.1f,0.1f,0.1f });
		}
		};


	// 応答システム初期化
	responseSystem_ = std::make_unique<ResponseSystem>();
	responseSystem_->Initialize(&GetCharacterParameterComponent(), objectComponent_.get());
	responseSystem_->GetHitResponse()->SetOwner(&objectComponent_->GetWorldTransform());


	// スペシャル攻撃
	special_ = std::make_unique<RangeBombingSpecial>();
	special_->Initialize(entity3DManager, entity2DManager, camera_);
	special_->SetParent(&GetObjectComponent()->GetWorldTransform());
	special_->SetInput(input);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
	rengeSp->SetRadius(50);
	rengeSp->SetReticleParent(&GetObjectComponent()->GetWorldTransform());
	rengeSp->Set(followCamera_, bulletManager_);

	// 攻撃系初期化
	InitAttack();

	// UI
	ui_ = std::make_unique<PlayerUI>();
	ui_->Initialize(input_, entity2DManager_, globalVariables_);
	ui_->SetHP(&characterParameterComponent_.HP());
	ui_->SetStamina(&characterParameterComponent_.Stamina());


	std::string nam = "MoveData";

	// ジャンプ
	globalVariables_->AddItem(nam, "ジャンプ力", moveGlovalData_.power_);
	globalVariables_->AddItem(nam, "ジャンプ上昇時の重力", moveGlovalData_.upGravity_);
	globalVariables_->AddItem(nam, "ジャンプ下降時の重力", moveGlovalData_.fallGravity_);
	
	// 移動
	globalVariables_->AddItem(nam, "移動最小速度", moveGlovalData_.minSpeed_);
	globalVariables_->AddItem(nam, "移動最大速度", moveGlovalData_.maxSpeed_);
	globalVariables_->AddItem(nam, "移動加速度", moveGlovalData_.speedAcceleration_);
	globalVariables_->AddItem(nam, "回転速度", moveGlovalData_.rotationSpeed_);
	globalVariables_->AddItem(nam, "スティック移動量変化", moveGlovalData_.isStickToSpeed_);
	globalVariables_->AddItem(nam, "空中での移動量変化", moveGlovalData_.isLimitAirSpeed_);
	globalVariables_->AddItem(nam, "空中移動量変化率", moveGlovalData_.airSpeedRate_);

	// ダッシュ
	globalVariables_->AddItem(nam, "ダッシュ時間", moveGlovalData_.dashMaxTime);
	globalVariables_->AddItem(nam, "ダッシュ加速度", moveGlovalData_.dashAcceleration);
	globalVariables_->AddItem(nam, "ダッシュ減速度", moveGlovalData_.dashFriction);
	globalVariables_->AddItem(nam, "ダッシュ初速度", moveGlovalData_.dashStartSpeed);
	globalVariables_->AddItem(nam, "ダッシュ中の重力", moveGlovalData_.isDashGravity);


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
	attackController_->Initialize(entity3DManager_,&characterParameterComponent_, this);
	attackController_->GetHitCounter().SetHitTimer(2.0f);
	

	//
	ApplyGlobalComboData("AttackComboData1", data1_);
	ApplyGlobalComboData("AttackComboData2", data2_);
	ApplyGlobalComboData("AttackComboData3", data3_);

	
	//
	ReloadComboData();

	// インプットハンドラー
	attackInputHander_ = std::make_unique<AttackInputHander>();
	attackInputHander_->AssignAttack();
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
	characterParameterComponent_.Update();

	// 移動コンポーネント更新
	moveComponent_->Update(MyGame::GameTime(), GetObjectComponent()->GetWorldTransform(),
		*GetObjectComponent()->GetRigidBodyComponent(), GetInput());

	// ステート
	stateMachine_->Update();

	//武器更新
	weapon_->GetObject3D()->GetWorldTransform().SetParent(Animetion::GetWorldMatrixOfJoint(GetObjectComponent()->GetObject3D()->model->modelData.skeleton, "rightHand", GetObjectComponent()->GetWorldTransform().worldMat_));
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
	ui_->SetIsTextRB(special_->GetIsSpecial());
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
		GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimetion("JumpStrat1", 0.01f);
	}

}

void NormalPlayer::Attack()
{
	bool isMove   = stateMachine_->GetCurrentMainState() == CharacterMainState::Move;
	bool isIdle   = stateMachine_->GetCurrentMainState() == CharacterMainState::Idle;
	bool isJump   = stateMachine_->GetCurrentMainState() == CharacterMainState::Jump;
	bool isAttack = stateMachine_->GetCurrentMainState() == CharacterMainState::Attack;


	// 攻撃
	if (isAttack) {
		GetAttackController()->SetIsAttack(true);
		GetAttackController()->GetComboSystem()->InputCombo(AttackInput::Light);
	}
	else if (isMove || isIdle || isJump) {
		GetAttackController()->SetIsAttack(true);
		stateMachine_->ChangeState(CharacterMainState::Attack);
		GetAttackController()->GetComboSystem()->StartCombo("Attack1");
	}

}

#pragma endregion //移動関係

#pragma region MyRegion

void NormalPlayer::ApplyGlobalVariables(){

	SetGlobalComboData("AttackComboData1", data1_);
	SetGlobalComboData("AttackComboData2", data2_);
	SetGlobalComboData("AttackComboData3", data3_);


	std::string nam = "MoveData";

	moveGlovalData_.power_ = globalVariables_->GetValue<float>(nam, "ジャンプ力");
	moveGlovalData_.upGravity_ = globalVariables_->GetValue<float>(nam, "ジャンプ上昇時の重力");
	moveGlovalData_.fallGravity_ = globalVariables_->GetValue<float>(nam, "ジャンプ下降時の重力");
	
	moveGlovalData_.minSpeed_			= globalVariables_->GetValue<float>(nam, "移動最小速度");
	moveGlovalData_.maxSpeed_			= globalVariables_->GetValue<float>(nam, "移動最大速度");
	moveGlovalData_.speedAcceleration_	= globalVariables_->GetValue<float>(nam, "移動加速度");
	moveGlovalData_.rotationSpeed_		= globalVariables_->GetValue<float>(nam, "回転速度");
	moveGlovalData_.isStickToSpeed_		= globalVariables_->GetValue<bool>(nam, "スティック移動量変化");
	moveGlovalData_.isLimitAirSpeed_	= globalVariables_->GetValue<bool>(nam, "空中での移動量変化");
	moveGlovalData_.airSpeedRate_		= globalVariables_->GetValue<float>(nam, "空中移動量変化率");


	moveGlovalData_.dashMaxTime			= globalVariables_->GetValue<float>(nam, "ダッシュ時間");
	moveGlovalData_.dashAcceleration	= globalVariables_->GetValue<float>(nam, "ダッシュ加速度");
	moveGlovalData_.dashFriction		= globalVariables_->GetValue<float>(nam, "ダッシュ減速度");
	moveGlovalData_.dashStartSpeed		= globalVariables_->GetValue<float>(nam, "ダッシュ初速度");
	moveGlovalData_.isDashGravity		= globalVariables_->GetValue<bool>(nam, "ダッシュ中の重力");


	// ジャンプに関するパラメータ設定
	moveComponent_->GetJumpSystem()->GetData().fallGravity_ = moveGlovalData_.fallGravity_;
	moveComponent_->GetJumpSystem()->GetData().upGravity_ = moveGlovalData_.upGravity_;
	moveComponent_->GetJumpSystem()->GetData().power_	= moveGlovalData_.power_;

	// 移動パラメータ設定
	moveComponent_->SetSpeed(moveGlovalData_.minSpeed_, moveGlovalData_.maxSpeed_);
	moveComponent_->GetMoveSystem()->GetData().speedAcceleration = moveGlovalData_.speedAcceleration_;
	moveComponent_->GetMoveSystem()->GetData().rotationSpeed	= moveGlovalData_.rotationSpeed_;
	moveComponent_->GetMoveSystem()->GetData().isStickToSpeed	= moveGlovalData_.isStickToSpeed_;
	moveComponent_->GetMoveSystem()->GetData().isLimitAirSpeed	= moveGlovalData_.isLimitAirSpeed_;
	moveComponent_->GetMoveSystem()->GetData().airSpeedRate		= moveGlovalData_.airSpeedRate_;

	// ダッシュパラメータ設定
	moveComponent_->GetDashSystem()->GetData().maxTime		= moveGlovalData_.dashMaxTime;
	moveComponent_->GetDashSystem()->GetData().acceleration = moveGlovalData_.dashAcceleration;
	moveComponent_->GetDashSystem()->GetData().friction		= moveGlovalData_.dashFriction;
	moveComponent_->GetDashSystem()->GetData().startSpeed	= moveGlovalData_.dashStartSpeed;
	moveComponent_->GetDashSystem()->GetData().isDashGravity = moveGlovalData_.isDashGravity;
}

void NormalPlayer::ApplyGlobalComboData(const std::string& name, ComboGlovalData& data)
{
	globalVariables_->CreateGroup(name);

	globalVariables_->AddItem(name, "ダメージ", data.damage);
	globalVariables_->AddItem(name, "ヒットボックス発生時間", data.hitBoxWindowStart_);
	globalVariables_->AddItem(name, "ヒットボックス生存時間", data.hitBoxLifeTime_);
	
	globalVariables_->AddItem(name, "Y方向ノックバック", data.isVerticalBoost_);
	globalVariables_->AddItem(name, "ノックバック力", data.knockbackPower);
	globalVariables_->AddItem(name, "Y方向ノックバック力", data.knockbackPowerY);
	globalVariables_->AddItem(name, "ノックバック持続時間", data.knockbackDuration_);

	globalVariables_->AddItem(name, "コンボ入力受付開始時間", data.stateInputStartTime);
	globalVariables_->AddItem(name, "コンボ入力受付終了時間", data.stateInputEndTime);
	globalVariables_->AddItem(name, "コンボ終了時間", data.stateEndTime);
	globalVariables_->AddItem(name, "コンボ移行時間", data.stateNextTime);
	globalVariables_->AddItem(name, "コンボ中の重力", data.isGravity);




	SetGlobalComboData(name, data);
}

void NormalPlayer::SetGlobalComboData(const std::string& name, ComboGlovalData& data)
{
	data.damage = globalVariables_->GetValue<float>(name, "ダメージ");
	data.hitBoxWindowStart_ = globalVariables_->GetValue<float>(name, "ヒットボックス発生時間");
	data.hitBoxLifeTime_ = globalVariables_->GetValue<float>(name, "ヒットボックス生存時間");
	data.isVerticalBoost_ = globalVariables_->GetValue<bool>(name, "Y方向ノックバック");
	data.knockbackPower = globalVariables_->GetValue<float>(name, "ノックバック力");
	data.knockbackPowerY = globalVariables_->GetValue<float>(name, "Y方向ノックバック力");
	data.knockbackDuration_ = globalVariables_->GetValue<float>(name, "ノックバック持続時間");

	data.stateInputStartTime = globalVariables_->GetValue<float>(name, "コンボ入力受付開始時間");
	data.stateInputEndTime = globalVariables_->GetValue<float>(name, "コンボ入力受付終了時間");
	data.stateEndTime = globalVariables_->GetValue<float>(name, "コンボ終了時間");
	data.stateNextTime = globalVariables_->GetValue<float>(name, "コンボ移行時間");
	data.isGravity = globalVariables_->GetValue<bool>(name, "コンボ中の重力");

}

void NormalPlayer::ReloadComboData()
{
	GetAttackController()->GetComboSystem()->ClearNode();
	// コンボデータ設定
	ComboData data1{};
	ComboData data2{};
	ComboData data3{};
	// ヒットボックスデータ
	HitBoxCollData hitData1{};
	HitBoxCollData hitData2{};
	HitBoxCollData hitData3{};
	hitData1.isEneble = true;
	hitData1.isLine = true;
	hitData1.layer = CollisionLayer::PlayerAttack;
	hitData1.tag = CollisionTag::PlayerAttack;
	hitData1.mask = CollisionLayer::Enemy;
	hitData1.name = "obbColl1";
	hitData1.offset = provisionalData_.collider1Pos;
	hitData1.size = provisionalData_.obbColliderSize;

	hitData1.damage = data1_.damage;
	hitData1.knockbackData.GetData().duration_ = data1_.knockbackDuration_;
	hitData1.knockbackData.GetData().power_ = data1_.knockbackPower;
	hitData1.knockbackData.GetData().verticalBoost_ = data1_.knockbackPowerY;
	hitData1.knockbackData.GetData().isVerticalBoost_ = data1_.isVerticalBoost_;


	hitData2 = hitData1;
	hitData2.name = "obbColl2";
	hitData2.offset = provisionalData_.collider2Pos;
	hitData2.size = provisionalData_.obbCollider2Size;

	

	hitData3 = hitData1;
	hitData3.name = "obb";
	hitData3.offset = { 0,0,3 };
	hitData3.size = { 10,10,10 };


	// コンボ１のデータ送る
	data1.hitBox.AddCollider(hitData3);
	
	hitData1.damage = data2_.damage;
	hitData1.knockbackData.GetData().duration_ = data2_.knockbackDuration_;
	hitData1.knockbackData.GetData().power_ = data2_.knockbackPower;
	hitData1.knockbackData.GetData().verticalBoost_ = data2_.knockbackPowerY;
	hitData1.knockbackData.GetData().isVerticalBoost_ = data2_.isVerticalBoost_;
	hitData2.damage = data2_.damage;
	hitData2.knockbackData.GetData().duration_ = data2_.knockbackDuration_;
	hitData2.knockbackData.GetData().power_ = data2_.knockbackPower;
	hitData2.knockbackData.GetData().verticalBoost_ = data2_.knockbackPowerY;
	hitData2.knockbackData.GetData().isVerticalBoost_ = data2_.isVerticalBoost_;


	// コンボ２のデータ送る
	data2.hitBox.AddCollider(hitData1);
	data2.hitBox.AddCollider(hitData2);
	
	hitData1.damage = data3_.damage;
	hitData1.knockbackData.GetData().duration_ = data3_.knockbackDuration_;
	hitData1.knockbackData.GetData().power_ = data3_.knockbackPower;
	hitData1.knockbackData.GetData().verticalBoost_ = data3_.knockbackPowerY;
	hitData1.knockbackData.GetData().isVerticalBoost_ = data3_.isVerticalBoost_;
	hitData2.damage = data3_.damage;
	hitData2.knockbackData.GetData().duration_ = data3_.knockbackDuration_;
	hitData2.knockbackData.GetData().power_ = data3_.knockbackPower;
	hitData2.knockbackData.GetData().verticalBoost_ = data3_.knockbackPowerY;
	hitData2.knockbackData.GetData().isVerticalBoost_ = data3_.isVerticalBoost_;


	// コンボ３のデータ送る
	data3.hitBox.AddCollider(hitData1);
	data3.hitBox.AddCollider(hitData2);
	

	// データ
	data1.hitBox.ClearUseHitBox();
	data1.hitBox.AddUseHitBox("obb");
	SetData(data1, data1_);
	data1.hitBox.SetPerent(&objectComponent_->GetObject3D()->GetWorldTransform());
	GetAttackController()->GetComboSystem()->AddComboNode("Attack1", data1);	// コンボ追加


	// データ
	data2.hitBox.ClearUseHitBox();
	data2.hitBox.AddUseHitBox("obbColl1");
	data2.hitBox.AddUseHitBox("obbColl2");
	SetData(data2, data2_);
	data2.hitBox.SetPerent(&weapon_->GetObject3D()->GetWorldTransform());
	GetAttackController()->GetComboSystem()->AddComboNode("Attack2", data2);	// コンボ追加


	// データ
	data3.hitBox.ClearUseHitBox();
	data3.hitBox.AddUseHitBox("obbColl1");
	data3.hitBox.AddUseHitBox("obbColl2");
	SetData(data3, data3_);
	data3.hitBox.SetPerent(&weapon_->GetObject3D()->GetWorldTransform());
	GetAttackController()->GetComboSystem()->AddComboNode("Attack3", data3);	// コンボ追加



	GetAttackController()->GetComboSystem()->ConnectCombo("Attack1", AttackInput::Light, "Attack2"); // コンボ連結
	GetAttackController()->GetComboSystem()->ConnectCombo("Attack1", AttackInput::Heavy, "Attack3"); // コンボ連結
	GetAttackController()->GetComboSystem()->ConnectCombo("Attack2", AttackInput::Light, "Attack3"); // コンボ連結

}

void NormalPlayer::SetData(ComboData& data,const ComboGlovalData& gData)
{
	data.hitBox.GetData().hitBpxWindowStart_ = gData.hitBoxWindowStart_;	// 発生時間
	data.hitBox.GetData().lifeTime_ = gData.hitBoxLifeTime_;				// 生成時間

	// 入力受付時間設定
	data.comboCondition.ConditionStartEnd(gData.stateInputStartTime, gData.stateInputEndTime);
	data.comboCondition.GetData().stateNextTime = gData.stateNextTime;
	data.comboCondition.GetData().stateEndTime = gData.stateEndTime;

	// 重力
	data.motion.GetData().isGravity_ = gData.isGravity;

}

#pragma endregion // そのほか
