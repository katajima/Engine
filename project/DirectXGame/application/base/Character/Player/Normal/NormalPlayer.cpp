#include "NormalPlayer.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"

#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <DirectXGame/application/base/Attack/Response/Response.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Bullet/base/BulletSpawn.h" 
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Special/Base/BaseSpecial.h>
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>

namespace Character {

	void NormalPlayer::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
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
		parameterComponent_->parameters->stamina.useRate = false; // スタミナの変化率を使用しない

		// 移動コンポーネント初期化
		moveComponent_ = std::make_unique<MovementComponent>();
		moveComponent_->Initialize(this, inputSystem, globalVariables, MovementComponent::ControlType::Manual, "");
		moveComponent_->SetMoveType(MoveType::ACCELERATE);
		moveComponent_->SetIsStickToSpeed(true);
		moveComponent_->SetControlType(MovementComponent::ControlType::Manual);
		moveComponent_->GetJumpSystem()->Data().power = 0.0f;
		moveComponent_->SetMaxJumpCount(1);
		moveComponent_->SetCamera(followCamera->GetUniqueCamera());

		// 回避成功などで使うスロー演出管理クラスを生成する
		slowMotionManager_ = std::make_unique<SlowMotionManager>();

		// 回避成功時に使うポストエフェクトを、プレイヤーが使用するカメラへ登録する
		dodgeSuccessEffect_ = std::make_unique<DodgeSuccessEffect>();
		dodgeSuccessEffect_->Initialize(followCamera->GetUniqueCamera());

		// 保存項目初期化
		InitializeBaseAddItem();

		// SphereColliderを追加
		auto sphere = std::make_unique<Engine::SphereCollider>();
		sphere->SetTag(CollisionTag::Player);
		sphere->SetLayer(CollisionLayer::ALL);
		sphere->SetCollisionMask(0xFFFFFFFF);
		sphere->radius = 1.0f; // 半径を適宜設定
		sphere->SetIsDebugLine(true);
		sphere->Enable();
		// コライダ追加
		GetColliderComponent()->AddCollider(std::move(sphere));
		// コンポーネント衝突インターフェース設定
		GetColliderComponent()->SetHitReceiver(this);

		// コライダ位置用トランスフォーム初期化
		worldCollider_ = std::make_unique<Engine::WorldTransform>();
		worldCollider_->Initialize();
		worldCollider_->parent_ = &GetObjectComponent()->GetWorldTransform();
		worldCollider_->translate_.y = 1.0f;

		// 衝突時のコールバック登録
		GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
			auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->GetOwner());
			if (!otherComponent) return;

			uint32_t otherId = otherComponent->GetUniqueId();

			// 敵との衝突応答
			hitResponse_->Hit(CollisionTag::Enemy, self, other);
			};

		// 衝突応答処理初期化
		hitResponse_ = std::make_unique<HitResponse>();
		hitResponse_->SetOwner(moveComponent_->GetMoveRequestSystem());

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
		RangeBombingSpecial* rangeSp = static_cast<RangeBombingSpecial*>(special_.get());
		rangeSp->SetRadius(50);
		rangeSp->SetReticleParent(&GetObjectComponent()->GetWorldTransform());
		rangeSp->Set(followCamera, bulletSpawn_.get());

		deathSystem_ = std::make_unique<DeathSystem>();
		deathSystem_->SetOwner(this);


		// 攻撃系初期化
		InitAttack();

		// UI
		ui_ = std::make_unique<PlayerUI>();
		ui_->Initialize(inputSystem, entityManager, globalVariables);
		ui_->SetCharacterParameter(GetCharacterParameterComponent());


		// コンテキストシステム
		contextSystem_ = std::make_unique<CharacterContextSystem>();
		contextSystem_->Initialize(this, inputSystem);
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
		subWeapon_->GetObject3D()->GetWorldTransform().rotate_ = { Math::DegreesToRadians(90),0.0f,Math::DegreesToRadians(180) };

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
		stateMachine_->RegisterState(CharacterMainState::Avoidance, [](BaseCharacter* p) {
			return std::make_unique<PlayerStateAvoidance>(p);
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

		// ロックオン対象を先に更新し、以降のカメラ更新で同じ対象を使う
		UpdateLockOn();

		// 回避成功後コンボの受付時間を毎フレーム減らす
		if (dodgeSuccessComboTimer_ > 0.0f) {
			dodgeSuccessComboTimer_ -= GetTime();
			if (dodgeSuccessComboTimer_ < 0.0f) {
				dodgeSuccessComboTimer_ = 0.0f;
			}
		}

		// スロー演出管理クラスを更新し、時間切れなら時間倍率を元へ戻す
		if (slowMotionManager_) {
			slowMotionManager_->Update(targetCharacters);
		}

		// 回避成功ポストエフェクトはスロー中でも実時間で解除したいので、固定デルタタイムで進める
		if (dodgeSuccessEffect_) {
			dodgeSuccessEffect_->Update(Engine::MyGame::BaseDeltaTime());
		}

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
		
		deathSystem_->Update(ctx.dt);

#ifdef _DEBUG
		entityManager->Get3DLineCommon()->GetLineMeshData().AddLine(ctx.position, ctx.position
			+ Vector3{ ctx.worldStickDirection.x,0, ctx.worldStickDirection.y } *20.0f);
#endif // _DEBUG

		// コライダのワールドトランスフォーム更新
		worldCollider_->Update();
		// コライダーコンポーネント更新
		GetObjectComponent()->GetColliderComponent()->UpdateAll(*worldCollider_.get());
		//武器更新
		weapon_->GetObject3D()->GetWorldTransform().SetParent(Engine::AnimationFunction::GetWorldMatrixOfJoint(GetObjectComponent()->GetObject3D()->GetModel()->GetModelData().skeleton, "DEF-hand.R", GetObjectComponent()->GetWorldTransform().worldMat_));
		weapon_->Update();

		// サブ武器更新
		if (auto* playerSubWeapon = static_cast<PlayerSubWeapon*>(subWeapon_.get())) {
			playerSubWeapon->Update(ctx.dt);
		}

		// UI更新
		ui_->SetImageLeftTopPosAndRatio(entityManager->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(),
			entityManager->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageRatio());
		ui_->Update(GetTime());

	}

	void NormalPlayer::UpdateLockOn() {
		if (!attackController_) {
			// 攻撃制御が未生成ならロックオンシステムも使えない
			return;
		}

		LockOnSystem* lockOnSystem = attackController_->GeyLockOnSysutem();
		if (!lockOnSystem) {
			// ロックオンシステムが無い場合はカメラだけ解除しておく
			if (followCamera) {
				followCamera->LockOn(nullptr);
			}
			return;
		}

		// キャラクターマネージャーから渡された最新の敵一覧をロックオン候補にする
		lockOnSystem->SetTargets(targetCharacters);

		const PlayerInputData inputData = inputSystem ? inputSystem->GetPlayerInputData() : PlayerInputData{};
		const BaseCharacter* target = lockOnSystem->UpdateLockOn(inputData.lockOnHeld);

		// フォローカメラへ対象のワールドトランスフォームを渡し、解除時はnullptrに戻す
		if (followCamera) {
			followCamera->LockOn(target ? target->GetConstWorldTransform() : nullptr);
		}
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

	void NormalPlayer::Dodge() {
		if (isSpecial) return;
		// 生存中かつ地上で動ける状態なら回避ステートへ移行する
		if (GetAlive() && moveComponent_->GetIsLanding()) {
			stateMachine_->ChangeState(CharacterMainState::Avoidance);
		}
	}

	void NormalPlayer::OnDodgeSuccess() {
		// 敵攻撃を回避中にすり抜けたら、専用開始コンボの受付時間を開く
		dodgeSuccessComboTimer_ = dodgeSuccessComboWindow_;

		// 成功した瞬間を分かりやすくするため、スロー演出リクエストを管理クラスへ渡す
		if (slowMotionManager_) {
			slowMotionManager_->Request(SlowMotionRequest::CreateDodgeSuccess());
		}

		// ポストエフェクトは画面効果クラスで再生する
		if (dodgeSuccessEffect_) {
			dodgeSuccessEffect_->Start();
		}
	}

	bool NormalPlayer::IsDodgeSuccessComboWindow() const {
		// 受付時間が残っている間だけ回避成功後コンボを開始できる
		return dodgeSuccessComboTimer_ > 0.0f;
	}

#pragma endregion //移動関係

#pragma region MyRegion
	void NormalPlayer::ReloadComboData() {
		Combo::System* comboSystem = GetAttackController()->GetComboSystem();
		// コンボノードクリア
		comboSystem->ClearNode();
		comboSystem->SetParentTransform("Player", &objectComponent_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("Weapon", &weapon_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("SubWeapon", &subWeapon_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("NoParent", nullptr);
		// ComboPlayer に登録されたノードを読み込むことで、攻撃データの追加をコード変更なしで反映する。
		comboSystem->Create("ComboPlayer");

	}

	void NormalPlayer::Reload() { ReloadComboData(); }


#pragma endregion // そのほか
}
