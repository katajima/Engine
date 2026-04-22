#include "NormalPlayer.h"

#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include "assert.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboEditor.h"

namespace Character {

	void NormalPlayer::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
	{
		this->entityManager = entityManager;	// エンティティ
		this->globalVariables = globalVariables;	// 保存項目
		this->camera = camera;					// カメラ
		this->inputSystem = inputSystem;						// 入力
		// オブジェクトコンポーネント追加
		objectComponent_ = std::make_unique<ObjectComponent>();
		objectComponent_->Initialize(entityManager, globalVariables,
			"PlayerBase", "testCharacter.gltf", true, true, this, Engine::ObjectModelType::kSkinning, false);
		// 保存項目追加
		CreateGroup("Player");

		objectComponent_->SetSRT({ 1,1,1 }, {}, position);					//　SRT設定
		objectComponent_->GetObject3D()->InitAnimationComponent();				// アニメーションコンポーネント初期化
		objectComponent_->SetIsUpdateColliderComponent(false);

		// キャラクターのパラメータコンポーネントを生成
		parameterComponent_ = std::make_unique<Character::ParameterComponent>();
		parameterComponent_->Initialize();

		// HP設定
		parameterComponent_->parameters->HP.Initiaize(200, 0, 200, 0);
		// スタミナ設定
		parameterComponent_->parameters->stamina.Initiaize(100, 0, 100, 10);


		// 移動コンポーネント初期化
		moveComponent_ = std::make_unique<MovementComponent>();
		moveComponent_->Initialize(this, inputSystem, globalVariables, MovementComponent::ControlType::Manual, "");
		moveComponent_->SetMoveType(MoveType::ACCELERATE);
		moveComponent_->SetIsStickToSpeed(true);
		moveComponent_->SetControlType(MovementComponent::ControlType::Manual);
		moveComponent_->GetJumpSystem()->Data().power = 0.0f;
		moveComponent_->SetMaxJumpCount(1);
		moveComponent_->SetCamera(followCamera->GetUniqueCamera());

		// 保存項目初期化
		InitializeBaseAddItem();

		// SphereColliderを追加
		auto sphere = std::make_unique<Engine::SphereCollider>();
		sphere->tag = CollisionTag::Player;
		sphere->layer = CollisionLayer::Player;
		sphere->collisionMask = 0xFFFFFFFF;
		sphere->radius = 1.0f; // 半径を適宜設定
		sphere->isDebugLine = true;
		sphere->Enable();
		// コライダ追加
		GetColliderComponent()->AddCollider(std::move(sphere));
		// コンポーネント衝突インターフェース設定
		GetColliderComponent()->SetHitReceiver(this);

		// コライダ位置用トランスフォーム初期化
		worldCollider_.Initialize();
		worldCollider_.parent_ = &GetObjectComponent()->GetWorldTransform();
		worldCollider_.translate_.y = 1.0f;

		// 衝突時のコールバック登録
		GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
			auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
			if (!otherComponent) return;

			uint32_t otherId = otherComponent->GetUniqueId();

			// 敵との衝突応答
			hitResponse_->Hit(CollisionTag::Enemy, self, other);

			// 壁との衝突応答
			//hitResponse_->HitWall(self, other);
			};

		// 衝突応答処理初期化
		hitResponse_ = std::make_unique<HitResponse>();
		hitResponse_->SetOwner(moveComponent_->GetResponseMoveSystem());

		// ヒットリアクションシステム初期化
		hitMotionSystem_ = std::make_unique<HitMotionSystem>();
		hitMotionSystem_->Initialize(this,effect);

		// 弾出現
		bulletSpawn_ = std::make_unique<BulletSpawn>();
		bulletSpawn_->Initialize(this, entityManager, globalVariables, camera, effect);

		// スペシャル攻撃
		special_ = std::make_unique<RangeBombingSpecial>();
		special_->Initialize(entityManager, camera);
		special_->SetOwner(this);
		special_->SetParent(&GetObjectComponent()->GetWorldTransform());
		special_->SetInputSystem(inputSystem);
		RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
		rengeSp->SetRadius(50);
		rengeSp->SetReticleParent(&GetObjectComponent()->GetWorldTransform());
		rengeSp->Set(followCamera, bulletSpawn_.get());

		// 攻撃系初期化
		InitAttack();

		// UI
		ui_ = std::make_unique<PlayerUI>();
		ui_->Initialize(inputSystem, entityManager, globalVariables);
		ui_->SetCharacterParameter(GetCharacterParameterComponent());


		// コンテキストシステム
		contextSystem_ = std::make_unique<CharacterContextSystem>();
		contextSystem_->Initialize(this, inputSystem);

		// オブジェクトコンポーネント追加
		objectComponentShadow_ = std::make_unique<ObjectComponent>();
		// オブジェクトインスタンシング初期化
		objectComponentShadow_->InitializeInstancing(entityManager, globalVariables, "PlayerBase", "plane.obj", "resources/Texture/smoke/no4.dds",
			false, false, this, Engine::Object3dInstansManager::TransparencyType::kYes);

		objectComponentShadow_->SetInstancingSRT({ 1.0f,1.0f,1.0f }, { Math::DegreesToRadians(-90),0.0f,0.0f }, { 0.0f,0.1f,0.0f });
		objectComponentShadow_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効

		// キャラクター行動ステート初期化
		InitStateMachine();
	}

	void NormalPlayer::InitAttack() {
		// 武器
		weapon_ = std::make_unique<PlayerWeapon>();
		weapon_->SetCharacter(this);
		weapon_->Initialize(inputSystem, entityManager, globalVariables, {}, camera);
		weapon_->GetObject3D()->GetWorldTransform().rotate_ = { Math::DegreesToRadians(90),0.0f,Math::DegreesToRadians(180) };

		// サブ武器
		subWeapon_ = std::make_unique<PlayerSubWeapon>();
		subWeapon_->SetCharacter(this);
		subWeapon_->Initialize(inputSystem, entityManager, globalVariables, {}, camera);

		// 戦闘
		attackController_ = std::make_unique<AttackController>();
		attackController_->Initialize(entityManager, globalVariables, GetCharacterParameterComponent(), this);
		attackController_->GetHitCounter().SetHitTimer(2.0f);

		// 
		ReloadComboData();
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
		stateMachine_->RegisterState(CharacterMainState::Fainting, [](BaseCharacter* p) {
			return std::make_unique<PlayerStateFainting>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Jump, [](BaseCharacter* p) {
			return std::make_unique<PlayerStateJump>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
			return std::make_unique<PlayerStateDie>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Damage, [](BaseCharacter* p) {
			return std::make_unique<PlayerStateDamage>(p);
			});

		stateMachine_->Init(this, CharacterMainState::Move);
	}
	// 更新
	void NormalPlayer::Update()
	{
		UpdateBaseGetValue(); //保存機能 基本値の更新
		ApplyGlobalVariables();

		// コンテキストシステム
		CharacterContext ctx = contextSystem_->CreateContext(this, GetTime());
		isCanJump = ctx.isCanJump;
		isSpecial = ctx.isSpecialAttacking;
#ifdef _DEBUG
		ImGui::Begin("Debug");
		if (ImGui::Button("SP")) {
			special_->SetGauge(100);
		}
		ImGui::End();
#endif // _DEBUG

		if (special_->GetPhese() == 1) {
			attackController_->IsStopHitTimer(true);
		}
		else {
			attackController_->IsStopHitTimer(false);
		}

		if (ctx.isStop) return;
		// 必殺技
		special_->Update();

		attackController_->GeyLockOnSysutem()->SetTargets(targetCharacters);
		// 攻撃制御更新
		attackController_->Update(ctx);
		// 応答システム
		hitMotionSystem_->Update(ctx.dt);
		
		// キャラクターパラメーター更新
		parameterComponent_->Update();

		if (parameterComponent_->GetHP() <= 0 && stateMachine_->GetCurrentMainState() != CharacterMainState::Die) {
			stateMachine_->ChangeState(CharacterMainState::Die);
		}

		// ステート
		stateMachine_->Update(ctx);

		// 移動コンポーネント更新
		moveComponent_->Update(GetObjectComponent()->GetWorldTransform(),
			*GetObjectComponent()->GetRigidBodyComponent(), ctx);

#ifdef _DEBUG
		entityManager->Get3DLineCommon()->GetLineMeshData().AddLine(ctx.position, ctx.position
			+ Vector3{ ctx.worldStickDirection.x,0, ctx.worldStickDirection.y } *20.0f);
#endif // _DEBUG

		// コライダのワールドトランスフォーム更新
		worldCollider_.Update();
		// コライダーコンポーネント更新
		GetObjectComponent()->GetColliderComponent()->UpdateAll(worldCollider_);
		//武器更新
		weapon_->GetObject3D()->GetWorldTransform().SetParent(Engine::AnimationFunction::GetWorldMatrixOfJoint(GetObjectComponent()->GetObject3D()->GetModel()->GetModelData().skeleton, "DEF-hand.R", GetObjectComponent()->GetWorldTransform().worldMat_));
		weapon_->Update();

		// サブ武器更新
		subWeapon_->Update();


		objectComponentShadow_->GetWorldTransform().translate_.x = GetWorldTransform().translate_.x;
		objectComponentShadow_->GetWorldTransform().translate_.z = GetWorldTransform().translate_.z;
		objectComponentShadow_->GetWorldTransform().translate_.y = 0.02f;
		objectComponentShadow_->Update();

		// UI更新
		ui_->SetImageLeftTopPosAndRatio(entityManager->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(),
			entityManager->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageRatio());
		ui_->Update(GetTime());

	}

	void NormalPlayer::Draw2D() {
		// UI表示
		ui_->SetIsTextmax(special_->GetIsSpecial());


		ui_->SetIsTextRB(special_->GetIsSpecial() || special_->IsAction());

		ui_->SetSpecialGaugeSize(static_cast<float>(special_->GetGauge()));

		ui_->Draw();
	}

#pragma region Move

	void NormalPlayer::Jump() {
		if (isSpecial) return;
		// 生きていてステートの状態が移動状態ならジャンプステートへ移動
		if (GetAlive() && isCanJump &&
			moveComponent_->GetIsJump() && moveComponent_->GetIsLanding()) {

			stateMachine_->ChangeState(CharacterMainState::Jump);
			GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimation("JumpStrat1", 0.01f);
		}
	}

#pragma endregion //移動関係

#pragma region MyRegion

	void NormalPlayer::RequestAttack(ActionInput input) {
		AttackContext ctx{};
		ctx.mainState = stateMachine_->GetCurrentMainState();
		ctx.isLanding = moveComponent_->GetIsLanding();
		ctx.isSpecial = isSpecial;
		ctx.stamina = static_cast<int>(parameterComponent_->GetStamina());

		const AttackDecision decision = AttackBranchResolver::Resolve(ctx, input);
		if (!decision.accepted) {
			return;
		}

		auto* ac = GetAttackController();
		if (!ac) {
			return;
		}

		if (decision.isComboInput) {
			ac->GetComboSystem()->InputCombo(decision.comboInput);
			return;
		}

		if (decision.shouldChangeToAttackState) {
			ac->SetIsAttack(true);
			stateMachine_->ChangeState(CharacterMainState::Attack);
			ac->GetComboSystem()->StartCombo(decision.startComboName);

			if (decision.staminaCost > 0) {
				parameterComponent_->Stamina().Add(-decision.staminaCost);
			}
		}
	}

	void NormalPlayer::ReloadComboData() {
		Combo::System* comboSystem = GetAttackController()->GetComboSystem();

		// コンボノードクリア
		comboSystem->ClearNode();
		comboSystem->SetParentTransform("Player", &objectComponent_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("Weapon", &weapon_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("SubWeapon", &subWeapon_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("NoParent", nullptr);
		// コンボ１のデータ送る
		comboSystem->CreateCombo("Attack1");
		// コンボ２のデータ送る
		comboSystem->CreateCombo("Attack2");
		// コンボ３のデータ送る
		comboSystem->CreateCombo("Attack3");
		// ジャンプコンボのデータ送る
		comboSystem->CreateCombo("JumpAttack");
		// コンボ4のデータ送る
		comboSystem->CreateCombo("Attack4");
		// コンボ5のデータ送る
		comboSystem->CreateCombo("Attack5");
		// コンボ6のデータ送る
		comboSystem->CreateCombo("Attack6");
		// コンボ7のデータ送る
		comboSystem->CreateCombo("Attack7");
		// コンボ8のデータ送る
		comboSystem->CreateCombo("Attack8");
		// コンボ9のデータ送る
		comboSystem->CreateCombo("Attack9");
		
		
		// コンボ10のデータ送る
		comboSystem->CreateCombo("Attack10");
		// コンボ11のデータ送る
		comboSystem->CreateCombo("Attack11");


		// コンボ9のデータ送る
		comboSystem->CreateCombo("SkillAttack01");

		// コンボ連結設定
		comboSystem->ConnectCombo("Attack1", ActionInput::LightAttack, "Attack2"); // コンボ連結
		comboSystem->ConnectCombo("Attack2", ActionInput::LightAttack, "Attack3"); // コンボ連結
		comboSystem->ConnectCombo("JumpAttack", ActionInput::LightAttack, "Attack2"); // コンボ連結	

		comboSystem->ConnectCombo("Attack3", ActionInput::LightAttack, "Attack4"); // コンボ連結
		comboSystem->ConnectCombo("Attack4", ActionInput::LightAttack, "Attack5"); // コンボ連結
		comboSystem->ConnectCombo("Attack5", ActionInput::LightAttack, "Attack6"); // コンボ連結
		comboSystem->ConnectCombo("Attack6", ActionInput::LightAttack, "Attack7"); // コンボ連結

		comboSystem->ConnectCombo("SkillAttack01", ActionInput::Skill, "SkillAttack01"); // コンボ連結
		comboSystem->ConnectCombo("SkillAttack01", ActionInput::LightAttack, "Attack6"); // コンボ連結
	
	
		comboSystem->ConnectCombo("Attack2", ActionInput::HeavyAttack, "Attack10"); // コンボ連結
		comboSystem->ConnectCombo("Attack10", ActionInput::HeavyAttack, "Attack11"); // コンボ連結

	}

	void NormalPlayer::Reload() { ReloadComboData(); }


#pragma endregion // そのほか
}