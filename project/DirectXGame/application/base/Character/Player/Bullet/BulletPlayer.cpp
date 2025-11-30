#include "BulletPlayer.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "assert.h"


///< summary>
/// 初期化
///</summary>
void BulletPlayer::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) {

	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目
	camera_ = camera;					// カメラ
	input_ = input;						// インプット



	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager_, globalVariables_, "PlayerBase", "origin.gltf", true, true, this, ObjectModelType::kSkinning);

	// 保存項目追加
	CreateGroup("Player");

	objectComponent_->SetSRT({ 1,1,1 }, {}, position);				//　SRT設定
	objectComponent_->GetObject3D()->InitAnimationComponent();				// アニメーションコンポーネント初期化
	objectComponent_->GetObject3D()->SetIsUpdateColliderComponent(false);		// コライダーコンポーネント内で更新するか


	// HP設定
	Parameters().HP.Initiaize(100, 0, 100, 0);
	Parameters().stamina.Initiaize(100, 0, 50, 0);
	HP() = 100; // 初期HP設定
	Parameters().speed = 20.0f;// 移動速度設定
	Parameters().jampPower = 100.0f;

	// 戦闘
	attackController_ = std::make_unique<AttackController>();
	attackController_->Initialize(entity3DManager,&characterParameterComponent_,this);


	// 移動コンポーネント初期化
	InitMoveComponent();
	moveComponent_->SetControlType(MovementComponent::ControlType::Manual);
	moveComponent_->SetMaxJumpCount(3);
	// 保存項目初期化
	InitializeBaseAddItem();

	// SphereColliderを追加
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::Player;
	sphere->layer = CollisionLayer::Player;
	sphere->collisionMask = 0xFFFFFFFF;
	sphere->radius = 2.0f; // 半径を適宜設定
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

		// 敵との衝突応答
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

		// 壁との衝突応答
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

		// 敵の攻撃状態なら
		if (enemy->GetCharacterStateMachine()->GetCurrentMainState() == CharacterMainState::Attack) {
			float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

			if (objectComponent_->GetContactRecord().CheckHistory(otherId, nowTime, 1.0f)) {
				return; // クールタイム中のため無視
			}

			objectComponent_->GetContactRecord().AddHistory(otherId, nowTime);

			// ダメージ
			AddDamage(DamageCalculator::ComputeDamage(*enemy->GetAttackController()->GetCombatStat(), *GetAttackController()->GetCombatStat(), 1.0f));
			followCamera_->GetUniqueCamera()->SetShake(0.25f, { 0.1f,0.1f,0.1f });
		}
		};


	// 武器管理クラス初期化
	weaponManager_ = std::make_unique<BulletWeaponManager>();
	weaponManager_->SetOwner(this);
	weaponManager_->SetEffect(effect_);
	weaponManager_->Initialize(bulletManager_,input,entity3DManager_,entity2DManager_,globalVariables_);

	// 右上
	weaponManager_->AddBulletWeapon("UPRIGHT",Vector3{2.5f,6,0}, Vector3{ 1.5f,4,8 });
	weaponManager_->AddBulletWeapon("UPLEFT",Vector3{-2.5f,6,0}, Vector3{ -1.5f,4,8 });
	weaponManager_->AddBulletWeapon("DOWNRIGHT",Vector3{2.5f,3,0}, Vector3{ 1.5f,3,8 });
	weaponManager_->AddBulletWeapon("DOWNLEFT",Vector3{-2.5f,3,0}, Vector3{ -1.5f,3,8 });



	// スペシャル攻撃
	special_ = std::make_unique<RangeBombingSpecial>();
	special_->Initialize(entity3DManager, entity2DManager, camera_);
	special_->SetParent(&GetObjectComponent()->GetWorldTransform());
	special_->SetInput(input);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
	rengeSp->SetRadius(100);
	rengeSp->SetReticleParent(&GetObjectComponent()->GetWorldTransform());
	rengeSp->Set(followCamera_, bulletManager_);

	
	// インプットハンドラー初期化
	attackInputHander_ = std::make_unique<AttackInputHander>();
	attackInputHander_->AssignAttack();

	// UI
	ui_ = std::make_unique<PlayerUI>();
	ui_->Initialize(input_, entity2DManager_, globalVariables_);
	ui_->SetHP(&characterParameterComponent_.HP());
	ui_->SetStamina(&characterParameterComponent_.Stamina());

	// キャラクター行動ステート初期化
	InitStateMachine();
};


// ステートマシーン初期化
void BulletPlayer::InitStateMachine() {
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->RegisterState(CharacterMainState::Idle, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateIdle>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateMove>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateAttack>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Special, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateSpecial>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Skill, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateSkill>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Defense, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateDefense>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Fainting, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateFainting>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Jump, [](BaseCharacter* p) {
		return std::make_unique<BulletPlayerStateJump>(p);
		});
	stateMachine_->Init(this, CharacterMainState::Move);
};

///< summary>
/// 更新
///</summary>
void BulletPlayer::Update() {
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
	// クリエイティブモード
	if (input_->IsTriggerKey(DIK_C)) {
		if (!isCreativeMode) {
			isCreativeMode = true;

		}
		else {
			isCreativeMode = false;
		}
	}
#endif // _DEBUG


	// ノーマル状態
	if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Up)) {
		weaponManager_->Normal();
	}
	// 貫通弾状態
	if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Down)) {
		weaponManager_->Penetration();
	}


	if (moveComponent_->GetIsLanding() &&
		stateMachine_->GetCurrentMainState() != CharacterMainState::Jump &&
		input_->GetGamePadLeftStick().Length() == 0) {
		moveComponent_->Velocity() = {};
	}



	// 移動コンポーネント更新
	moveComponent_->Update(MyGame::GameTime(), GetObjectComponent()->GetWorldTransform(),
		*GetObjectComponent()->GetRigidBodyComponent(), GetInput());


	// ワールドトランスフォーム更新
	GetObjectComponent()->GetWorldTransform().Update();

	// コライダのワールドトランスフォーム更新
	worldCollider_.Update();

	// コライダーコンポーネント更新
	GetObjectComponent()->GetColliderComponent()->UpdateAll(worldCollider_);

	// 必殺技
	special_->Update();
	
#ifdef _DEBUG
	ui_->SetImageLeftTopPosAndRatio(entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageleftTopPos(), entity3DManager_->GetObject3dCommon()->GetDxCommon()->GetPostEffectManager()->GetImageRatio());
#endif // _DEBUG

	// UI更新
	ui_->Update();



	// 武器マネージャー更新
	LockOn();
	weaponManager_->Update();

	// キャラクターパラメーター更新
	characterParameterComponent_.Update();

	// ステート
	stateMachine_->Update();
};

/// <summary>
/// エフェクトの描画
/// </summary>
void BulletPlayer::DrawEffect() {
};

/// <summary>
/// 描画2d
/// </summary>
void BulletPlayer::Draw2D() {

	// UI表示
	ui_->SetIsTextmax(special_->GetIsSpecial());	
	ui_->SetIsTextRB(special_->GetIsSpecial());
	ui_->SetSpecialGaugeSize(static_cast<float>(special_->GetGauge()));
	ui_->Draw();
};

// 移動処理
void BulletPlayer::Move() {
	// 重力
	if (stateMachine_->GetCurrentMainState() != CharacterMainState::Jump) {
		Velocity() = { 0,0,0 };
	}


	bool is = stateMachine_->GetCurrentMainState() == CharacterMainState::Move;
	bool is2 = stateMachine_->GetCurrentMainState() == CharacterMainState::Idle;

	if (is || is2) {
		moveComponent_->SetSpeed(0.1f ,Parameters().speed);
		moveComponent_->SetCamera(followCamera_->GetUniqueCamera());
		moveComponent_->SetCanMove(true);
	}
};

// ジャンプ
void BulletPlayer::Jump() {
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
};

// 攻撃
void BulletPlayer::Attack() {
	bool is = stateMachine_->GetCurrentMainState() == CharacterMainState::Move;
	bool is2 = stateMachine_->GetCurrentMainState() == CharacterMainState::Idle;
	bool is3 = stateMachine_->GetCurrentMainState() == CharacterMainState::Jump;


	if (stateMachine_->GetCurrentMainState() == CharacterMainState::Attack) {}
	else if (is || is2 || is3) {	// 弾発射
		stateMachine_->ChangeState(CharacterMainState::Attack);

		weaponManager_->GetBulletWeapon("UPRIGHT")->Shoot();
		weaponManager_->GetBulletWeapon("UPLEFT")->Shoot();
		weaponManager_->GetBulletWeapon("DOWNRIGHT")->Shoot();
		weaponManager_->GetBulletWeapon("DOWNLEFT")->Shoot();
	}
};

void BulletPlayer::LockOn() {

	// ターゲット選別

	
	std::vector<BaseEnemy*> enemys;
	
	// 範囲内に入った敵を抽出
	for (auto& target : targetCharacters_) {
		Vector2 posEne = target->GetObjectComponent()->GetScreenPosition();
		float direction = target->GetTargetDistance();
		Vector2 diff = Vector2{ 640,360 } - posEne;
		float length = diff.Length();
		target->SetIsLockOn(false);
		if (length <= 300.0f && target->GetAlive() && direction < 300) {

			

			enemys.push_back(target);
		}
	}

	// --- 範囲内に敵がいない場合は処理を抜ける ---
	if (enemys.empty()) {
		weaponManager_->SetTargets({});
		return;
	}

	// 範囲内の入った敵を近い順にソートしてそれぞれの銃にターゲットを指定

	// --- 距離が近い順にソート ---
	std::sort(enemys.begin(), enemys.end(),
		[](BaseEnemy* a, BaseEnemy* b) {
			return a->GetTargetDistance() < b->GetTargetDistance();
		});

	// --- ソート後の敵リストを武器マネージャーへ渡す ---
	weaponManager_->SetTargets(enemys);
};