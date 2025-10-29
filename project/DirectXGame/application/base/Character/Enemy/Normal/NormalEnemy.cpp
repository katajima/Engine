#include "NormalEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

void NormalEnemy::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;

	// サイズ
	Vector3 size = { 1.7f,1.7f,1.7f };

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();

	objectComponent_->InitializeInstancing(entity3DManager_, globalVariables_, "enemy" + std::to_string(id_), "enemy.gltf", "", true, true, this);
	objectComponent_->SetInstancingSRT(size, {}, position);

	//objectComponent_->Initialize(entity3DManager_, globalVariables_, "enemy" + std::to_string(id_), "enemy.gltf", true, true, this);
	//objectComponent_->SetSRT(size, {}, position);

	objectComponent_->GetColliderComponent()->SetHitReceiver(this);

	CreateGroup("enemy");

	// 移動コンポーネント初期化
	InitMoveComponent();


	// SphereColliderを追加
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::Enemy;
	sphere->layer = CollisionLayer::Enemy;
	sphere->radius = 3.0f; // 半径を適宜設定
	GetColliderComponent()->AddCollider(std::move(sphere));

	// コールバック登録（例：プレイヤーと衝突したらダメージ）
	GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		// プレイヤーかチェック
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		if (other->tag == CollisionTag::Enemy) {
			Vector3 pushVec;
			if (self->ResolveCollision(*other, pushVec)) {
				pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {
					// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					GetWorldTransform().translate_ += pushVec * 0.5f;
				}

				GetWorldTransform().Update();
			}

			isStopMove_ = true;

		}

		if (other->tag == CollisionTag::Player) {
			// ここにダメージ処理などを書く
			std::cout << "敵がプレイヤーに当たった！" << std::endl;

			Vector3 pushVec;
			if (self->ResolveCollision(*other, pushVec)) {
				pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {
					// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					GetWorldTransform().translate_ += pushVec * 0.5f;
				}

				GetWorldTransform().Update();
			}
		}
		if (other->tag == CollisionTag::Wall) {
			Vector3 pushVec;
			if (self->ResolveCollision(*other, pushVec)) {
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {
					// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					GetWorldTransform().translate_ += pushVec * 0.5f;
				}
				//acceleration_.y = 0;
				Velocity().y = 0;
				//velocity_.y = 0;
				//flags_.isGrounded = true;
				GetWorldTransform().Update();
			}
		}
		};

	// 視野
	visionComponent_ = std::make_unique<VisionComponent>();
	visionComponent_->SetAlertView(120.0f, 100.0f);
	visionComponent_->SetCombatView(90.0f, 100.0f);
	visionComponent_->SetLineCommon(entity3DManager_->Get3DLineCommon());
	visionComponent_->raycastFunc = [this](Vector3 origin, Vector3 dir, float maxDist)-> bool {return false; };


	// ヒットモーション
	hitMotionComponent_ = std::make_unique<HitMotionComponent>();
	hitMotionComponent_->Init(0.1f, { 2.5f,2.2f,2.5f });


	objectComponent_->GetObjectStateFlags().isAlive = true;

	// パラメーター初期化
	Parameters().HP.Initiaize(100, 0, 100, 0);
	Parameters().speed = 3.0f;
	Parameters().strength = 10.0f;


	// 戦闘中の倍率・軽減率を扱う
	combatStatComponent_ = std::make_unique<CombatStatComponent>();
	combatStatComponent_->Initialize(&characterParameterComponent_);


	// 保存項目初期化
	InitializeBaseAddItem();

	Initialize2D();
	InitParticle();


	GetWorldTransform().Update();

	// ステートマシーン初期化
	InitStateMachine();
}

void NormalEnemy::InitStateMachine() {
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


	if (GetObjectComponent()) {


		if (isStopMove_) {
			stopMoveTimer_ += GetTime();
			if (stopMoveTimer_ >= 1.0f) {
				stopMoveTimer_ = 0.0f;
				isStopMove_ = false;
			}
		}

		UpdateBaseGetValue();
		// ステート
		stateMachine_->Update();

		//HitStpoTime();
		if (GetHP() <= 0) {
			if (GetAlive() == true) {

			}
			objectComponent_->GetObjectStateFlags().isLockonTarget = false;
			objectComponent_->GetObjectStateFlags().isAlive = false;
		}
		else {
			// 移動
			moveComponent_->AddMove(GetTime(), GetAlive(), objectComponent_->GetWorldTransform());
			// 着地
			moveComponent_->Landing(objectComponent_->GetWorldTransform(), *objectComponent_->GetRigidBodyComponent());
			// ヒット
			hitMotionComponent_->Update(GetTime(), objectComponent_.get());
			// 視野
			visionComponent_->Update(GetTime(), objectComponent_->GetWorldTransform().GetWorldPosition(), moveComponent_->GetDirection(), player_->GetWorldTransform().translate_);
			// 移動制限
			LimitMove(-Vector3{ 200,200,200 }, Vector3{ 200,200,200 });
			// 更新

			objectComponent_->GetWorldTransform().Update();
		}
	}
}

void NormalEnemy::DrawEffect()
{

}

void NormalEnemy::Draw2D()
{
	if (GetObjectComponent() == nullptr) { return; }

	if (GetIsLockOn()) {
		Vector2 screenPos = objectComponent_->GetScreenPosition();
		icon_lockOn->SetPosition(screenPos + Vector2{ 0.0f,-40.0f });

		icon_lockOn->Update();
		icon_lockOn->Draw();
	}
}

void NormalEnemy::SetPlayer(BasePlayer* player)
{
	player_ = player;
}

void NormalEnemy::Emit()
{
	worldEffect_.Update();


	effect_->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
}

void NormalEnemy::Move()
{

	// 回転と移動量の設定
	if (stateMachine_->GetCurrentMainState() != CharacterMainState::Attack && !isStopMove_) {
		if (Distance(GetTargetPos(), objectComponent_->GetWorldTransform().GetWorldPosition()) <= 10) {
			Parameters().speed = 0;
		}
		else {
			Parameters().speed = 5.0f;
		}
		// 移動
		DirectionMove(Parameters().speed);
	}
}

void NormalEnemy::Jump()
{

}

void NormalEnemy::Attack() {

}

void NormalEnemy::InitParticle()
{
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	worldEffect_.Initialize();
	worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
	worldEffect_.translate_ = { 0,1,0 };
}
