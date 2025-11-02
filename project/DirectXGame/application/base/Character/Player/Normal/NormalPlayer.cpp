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
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	camera_ = camera;
	input_ = input;
	
	
	
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager_, globalVariables_, "PlayerBase", "origin.gltf", true, true,this, ObjectModelType::kSkinning);


	CreateGroup("Player");

	objectComponent_->SetSRT({1,1,1}, {}, position);				//　SRT設定
	objectComponent_->GetObject3D()->InitAnimationComponent();				// アニメーションコンポーネント初期化
	objectComponent_->GetObject3D()->SetIsUpdateColliderComponent(false);		// コライダーコンポーネント内で更新するか


	// HP設定
	Parameters().HP.Initiaize(100, 0, 100, 0);
	Parameters().stamina.Initiaize(100, 0, 50, 0);
	HP() = 100; // 初期HP設定
	Parameters().speed = 20.0f;// 移動速度設定
	Parameters().jampPower = 100.0f;
	
	// 戦闘中の倍率・軽減率を扱う
	combatStatComponent_ = std::make_unique<CombatStatComponent>();
	combatStatComponent_->Initialize(&characterParameterComponent_);


	// 移動コンポーネント初期化
	InitMoveComponent();
	moveComponent_->SetMaxJumpCount(3);
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


		Vector3 pushVec;
		if (other->tag == CollisionTag::Enemy) {
			if (self->ResolveCollision(*other, pushVec)) {
				pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {

				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					GetWorldTransform().translate_ += pushVec * 0.5f;

				}
				GetWorldTransform().Update();
			}
		}
		if (other->tag == CollisionTag::Wall) {
			if (self->ResolveCollision(*other, pushVec)) {
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {

				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					GetWorldTransform().translate_ += pushVec * 0.5f;

				}
				Velocity().y = 0;

				//acceleration_.y = 0;
				GetWorldTransform().Update();
			}
		}
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		if (!enemy) return;
		if (enemy->GetCharacterStateMachine()->GetCurrentMainState() == CharacterMainState::Attack) {
			float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

			if (objectComponent_->GetContactRecord().CheckHistory(otherId, nowTime, 1.0f)) {
				return; // クールタイム中のため無視
			}

			objectComponent_->GetContactRecord().AddHistory(otherId, nowTime);


			AddDamage(DamageCalculator::ComputeDamage(*enemy->GetCombatStatComponent(), *GetCombatStatComponent(), 1.0f));
			followCamera_->GetUniqueCamera()->SetShake(0.25f, { 0.1f,0.1f,0.1f });
		}
		};



	// スペシャル攻撃
	special_ = std::make_unique<RangeBombingSpecial>();
	special_->Initialize(entity3DManager, entity2DManager, camera_);
	special_->SetParent(&GetObjectComponent()->GetWorldTransform());
	special_->SetInput(input);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
	rengeSp->SetRadius(100);
	rengeSp->SetReticleParent(&GetObjectComponent()->GetWorldTransform());
	rengeSp->Set(followCamera_, bulletManager_);

	// 武器
	weapon_ = std::make_unique<PlayerWeapon>();
	weapon_->SetCharacter(this);
	weapon_->Initialize(input_, entity3DManager_, nullptr, globalVariables_, {}, camera);
	weapon_->GetObject3D()->GetWorldTransform().rotate_ = { Math::DegreesToRadians(-90),0.0f,0.0f };
	weapon_->GetHitData().hitTime.maxT = 2.0f;

	attackInputHander_ = std::make_unique<AttackInputHander>();
	attackInputHander_->AssignAttack();

	// UI
	ui_ = std::make_unique<PlayerUI>();
	ui_->Initialize(input_, entity2DManager_, globalVariables_);
	ui_->SetHP(&characterParameterComponent_.HP());
	ui_->SetStamina(&characterParameterComponent_.Stamina());

	// キャラクター行動ステート初期化
	InitStateMachine();
}

// ステート初期化and追加
void NormalPlayer::InitStateMachine() {
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
	// ヒットデータの更新
	weapon_->GetHitData().Update(MyGame::GameTime()); // 武器のヒットデータ更新
	//武器更新
	weapon_->GetObject3D()->GetWorldTransform().SetParent(Animetion::GetWorldMatrixOfJoint(GetObjectComponent()->GetObject3D()->model->modelData.skeleton, "rightHand", GetObjectComponent()->GetWorldTransform().worldMat_));
	weapon_->Update();

	// 移動コンポーネント移動
	moveComponent_->AddMove(MyGame::GameTime(), GetAlive(), GetObjectComponent()->GetWorldTransform());
	// 移動コンポーネント着地状態か
	moveComponent_->Landing(GetObjectComponent()->GetWorldTransform(), *GetObjectComponent()->GetRigidBodyComponent());
	
	
	// クリエイティブモードではないなら移動制限を付ける
	if (!isCreativeMode) {
		// 移動制限
		LimitMove(-Vector3{ 200,200,200 }, Vector3{ 200,200,200 });
	}

	// ワールドトランスフォーム更新
	GetObjectComponent()->GetWorldTransform().Update();
	
	// コライダのワールドトランスフォーム更新
	worldCollider_.Update();

	// コライダーコンポーネント更新
	GetObjectComponent()->GetColliderComponent()->UpdateAll(worldCollider_);



//#ifdef _DEBUG
	ui_->SetImageLeftTopPosAndRatio(entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(), entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageRatio());
//#endif // _DEBUG

	// UI更新
	ui_->Update();

	// キャラクターパラメーター更新
	characterParameterComponent_.Update();

	// ステート
	stateMachine_->Update();
}

#pragma region Draw

void NormalPlayer::DrawEffect()
{
}

void NormalPlayer::Draw2D()
{
	
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

	if (is || is2) {
		moveComponent_->SetSpeed(Parameters().speed);
		moveComponent_->SetCamera(followCamera_->GetUniqueCamera());
		moveComponent_->Move(GetObjectComponent()->GetWorldTransform(), input_);
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
		GetObjectComponent()->GetRigidBodyComponent()->Velocity().y = 0;
		moveComponent_->DecrementJumpCount(); // ジャンプ回数減少
		GetObjectComponent()->GetRigidBodyComponent()->AddForce({ 0,characterParameterComponent_.parameters_.jampPower,0 });
		GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimetion("JumpStrat1", 0.01f);
	}

}

void NormalPlayer::Attack()
{
	bool is = stateMachine_->GetCurrentMainState() == CharacterMainState::Move;
	bool is2 = stateMachine_->GetCurrentMainState() == CharacterMainState::Idle;
	bool is3 = stateMachine_->GetCurrentMainState() == CharacterMainState::Jump;


	if (stateMachine_->GetCurrentMainState() == CharacterMainState::Attack) {
		weapon_->InputCombo(AttackInput::Light);
	}
	else if (is || is2 || is3) {
		stateMachine_->ChangeState(CharacterMainState::Attack);
		weapon_->StartCombo("Attack1");
	}

}

#pragma endregion //移動関係

#pragma region MyRegion

void NormalPlayer::ApplyGlobalVariables(){}

#pragma endregion // そのほか
