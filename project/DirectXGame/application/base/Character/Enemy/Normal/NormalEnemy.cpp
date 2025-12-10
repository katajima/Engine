#include "NormalEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

void NormalEnemy::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目

	// サイズ
	Vector3 size = { 1.7f,1.7f,1.7f };

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	// オブジェクトインスタンシング初期化
	objectComponent_->InitializeInstancing(entity3DManager_, globalVariables_, "enemy" + std::to_string(id_), "enemy.gltf", "", true, true, this);
	objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定
	objectComponent_->GetColliderComponent()->SetHitReceiver(this);	// インターフェース設定
		
	// 保存項目追加
	CreateGroup("enemy");

	// 移動コンポーネント初期化
	moveComponent_ = std::make_unique<MovementComponent>();
	moveComponent_->Initialize(globalVariables_, MovementComponent::ControlType::Manual, "_Enemy");
	moveComponent_->SetControlType(MovementComponent::ControlType::Auto);
	// SphereColliderを追加
	auto sphere = std::make_unique<SphereCollider>();
	sphere->Enable();					// コライダ有効
	sphere->tag = CollisionTag::Enemy;	// タグ設定
	sphere->layer = CollisionLayer::Enemy;// レイヤー設定
	sphere->radius = 3.0f; // 半径を適宜設定
	GetColliderComponent()->AddCollider(std::move(sphere));	// コライダ追加

	// コールバック登録（例：プレイヤーと衝突したらダメージ）
	GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		// プレイヤーかチェック
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		//  敵同士の衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Enemy, self, other);

		// プレイヤーとの衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Player,self,other);

		// 壁との衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Wall,self,other);

		if (other->tag == CollisionTag::PlayerAttack)
			responseSystem_;
		};

	// 視野
	visionComponent_ = std::make_unique<VisionComponent>();
	visionComponent_->SetAlertView(120.0f, 100.0f);	
	visionComponent_->SetCombatView(90.0f, 100.0f);
	visionComponent_->SetLineCommon(entity3DManager_->Get3DLineCommon());
	visionComponent_->raycastFunc = [this](Vector3 origin, Vector3 dir, float maxDist)-> bool {return false; };


	// 攻撃応答システムクラス初期化
	responseSystem_ = std::make_unique<ResponseSystem>();
	responseSystem_->Initialize(&GetCharacterParameterComponent(),objectComponent_.get());
	responseSystem_->GetHitResponse()->SetOwner(&objectComponent_->GetWorldTransform());

	// オブジェクト状態生存
	objectComponent_->GetObjectStateFlags().isAlive = true;

	// パラメーター初期化
	Parameters().HP.Initiaize(100, 0, 100, 0);
	Parameters().speed = 3.0f;
	Parameters().strength = 10.0f;

	moveComponent_->GetMoveSystem()->GetData().maxSpeed = Parameters().speed;


	// 戦闘中の倍率・軽減率を扱う
	attackController_ = std::make_unique<AttackController>();
	attackController_->Initialize(entity3DManager,globalVariables_,&characterParameterComponent_,this);

	

	// 保存項目初期化
	InitializeBaseAddItem();
	// スプライト初期化
	Initialize2D();
	// パーティクル初期化
	InitParticle();

	// トランスフォーム更新
	GetWorldTransform().Update();

	// ステートマシーン初期化
	InitStateMachine();
}

void NormalEnemy::InitStateMachine() {
	// ステートマシーン初期化
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
		return std::make_unique<EnemyStateMove>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
		return std::make_unique<EnemyStateAttack>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
		return std::make_unique<EnemyStateDie>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Special, [](BaseCharacter* p) {
		return std::make_unique<EnemyStateSpecial>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Fainting, [](BaseCharacter* p) {
		return std::make_unique<EenmyStateFainting>(p);
		});


	stateMachine_->Init(this, CharacterMainState::Move);
}

void NormalEnemy::Update()
{
	if (GetObjectComponent() == nullptr) { return; }
	assert(this);


	// 移動を止めているなら
	if (isStopMove_) {

		// 移動Stop時間更新
		stopMoveTimer_ += GetTime();

		// 時間に達したら動き出す
		if (stopMoveTimer_ >= 1.0f) {
			stopMoveTimer_ = 0.0f;
			isStopMove_ = false;
		}
	}

	// 保存項目更新
	UpdateBaseGetValue();

	// ステート
	stateMachine_->Update();

	// HPが0以下なら
	if (GetHP() <= 0) {
		GetObjectComponent()->GetObjectStateFlags().isLockonTarget = false;
		GetObjectComponent()->GetObjectStateFlags().isAlive = false;
	}
	else {
		// 移動コンポーネント更新
		moveComponent_->Update(GetTime(), GetObjectComponent()->GetWorldTransform(), *GetObjectComponent()->GetRigidBodyComponent(),nullptr);
		// 視野
		visionComponent_->Update(GetTime(), GetObjectComponent()->GetWorldTransform().GetWorldPosition(), moveComponent_->GetDirection(), player_->GetWorldTransform().translate_);
		// 応答システム
		responseSystem_->Update(GetTime());
	}
}

void NormalEnemy::DrawEffect()
{

}

void NormalEnemy::Draw2D()
{
	if (GetObjectComponent() == nullptr) { return; }

	// ロックオンされているなら
	if (GetIsLockOn()) {
		Vector2 screenPos = objectComponent_->GetScreenPosition();	// スクリーン座標取得
		icon_lockOn->SetPosition(screenPos + Vector2{ 0.0f,-40.0f });// 位置設定

		icon_lockOn->Update();	// 更新
		icon_lockOn->Draw();	// 描画
	}
}

void NormalEnemy::SetPlayer(BasePlayer* player)
{
	player_ = player;
}

void NormalEnemy::Emit()
{
	// エフェクト座標更新
	worldEffect_.Update();

	// 各エフェクト出現
	effect_->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
}

void NormalEnemy::Move()
{
	// 移動
	DirectionMove(Parameters().speed);
}

void NormalEnemy::Jump()
{

}

void NormalEnemy::Attack() {

}

void NormalEnemy::InitParticle()
{
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	// エフェクト用のトランスフォーム初期化
	worldEffect_.Initialize();
	worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
	worldEffect_.translate_ = { 0,1,0 };
}

void NormalEnemy::AttackByCrowdCommand()
{
	// ターゲットとの距離
	float dist = GetTargetDistance();
	if (dist < 5.0f) {
		// 攻撃ステートに遷移
		stateMachine_->ChangeState(CharacterMainState::Attack);
	}

}
