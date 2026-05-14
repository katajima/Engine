#include "BaseEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"
#include "DirectXGame/application/base/Character/Crowd/CrowdManager.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Attack/Response/Response.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Bullet/base/BulletSpawn.h" 
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>
#include <DirectXGame/application/base/Attack/HitBox/HitBox.h>

namespace Character {
	BaseEnemy::~BaseEnemy() = default;


	Vector3 BaseEnemy::GetTargetPos()
	{
		// ターゲット位置
		return GetTarget()->GetWorldPosition();
	}

	float BaseEnemy::GetTargetDistance()
	{
		// ターゲットとの距離
		return GetObjectComponent()->GetWorldTransform().GetWorldPosition().DistanceXZ(GetTarget()->GetWorldPosition());
	}

	Vector3 BaseEnemy::TargetDirection()
	{
		// ターゲットの方向
		return Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
	}

#pragma region

	void BaseEnemy::InitShadowObjectComponent(const std::string& charaName)
	{
		// オブジェクトコンポーネント追加
		objectComponentShadow_ = std::make_unique<ObjectComponent>();
		// オブジェクトインスタンシング初期化
		objectComponentShadow_->InitializeInstancing(entityManager, globalVariables, charaName + std::to_string(id_), "plane.obj", "resources/Texture/smoke/no4.dds",
			false, false, this, Engine::ObjectInstans::TransparencyType::kYes);
		objectComponentShadow_->SetColor({ 0,0,0,1.0f });
		objectComponentShadow_->SetInstancingSRT({ 1.0f,1.0f,1.0f }, { Math::DegreesToRadians(-90),0.0f,0.0f }, { 0.0f,0.2f,0.0f });
		objectComponentShadow_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効
		objectComponentShadow_->Update();
	}


	void BaseEnemy::BaseInitialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
		const std::string& modelName, const std::string& charaName, float colliderRadius) {

		this->entityManager = entityManager;	// エンティティ3d
		this->globalVariables = globalVariables;	// 保存項目

		// オブジェクトコンポーネント追加
		objectComponent_ = std::make_unique<ObjectComponent>();
		// オブジェクトインスタンシング初期化
		objectComponent_->InitializeInstancing(entityManager, globalVariables, charaName + std::to_string(id_), modelName, "", true, true, this
			, Engine::ObjectInstans::TransparencyType::kNo, false);
		objectComponent_->GetColliderComponent()->SetHitReceiver(this);	// インターフェース設定	
		objectComponent_->SetIsUpdateColliderComponent(false);		// コライダーコンポーネント内で更新するか


		// コライダ位置用トランスフォーム初期化
		worldCollider_ = std::make_unique<Engine::WorldTransform>();
		worldCollider_->Initialize();
		worldCollider_->parent_ = &GetObjectComponent()->GetWorldTransform();
		worldCollider_->translate_.y = colliderRadius / 2.0f;

		// キャラクターのパラメータコンポーネントを生成
		parameterComponent_ = std::make_unique<Character::ParameterComponent>();
		parameterComponent_->Initialize();

		// 移動コンポーネント初期化
		moveComponent_ = std::make_unique<MovementComponent>();
		moveComponent_->UseGlobal(false);
		moveComponent_->Initialize(this, nullptr, globalVariables, MovementComponent::ControlType::Manual, "_" + charaName);
		moveComponent_->SetControlType(MovementComponent::ControlType::Auto);
		moveComponent_->GetMoveSystem()->Data().minSpeed = -100;
		// 保存項目追加
		CreateGroup(charaName);

		// 保存項目初期化(キャラクター全体)
		InitializeBaseAddItem();
		// 保存項目初期化(敵全体)
		InitializeBaseEnemyAddItem();

		// SphereColliderを追加
		auto sphere = std::make_unique<Engine::SphereCollider>();
		sphere->Enable();					// コライダ有効
		sphere->tag = CollisionTag::Enemy;	// タグ設定
		sphere->layer = CollisionLayer::ALL;// レイヤー設定
		sphere->collisionMask = 0xFFFFFFFF;
		sphere->radius = colliderRadius; // 半径を適宜設定
		GetColliderComponent()->AddCollider(std::move(sphere));	// コライダ追加

		// コールバック登録（例：プレイヤーと衝突したらダメージ）
		GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
			// プレイヤーかチェック
			auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
			if (!otherComponent) return;

			if (other->tag == CollisionTag::Enemy) {
				isStopping_ = true;
			}

			//  敵同士の衝突応答
			hitResponse_->Hit(CollisionTag::Enemy, self, other);
			};


		
		// 衝突応答処理初期化
		hitResponse_ = std::make_unique<HitResponse>();
		hitResponse_->SetOwner(moveComponent_->GetMoveRequestSystem());

		// ヒットリアクションシステム初期化
		hitMotionSystem_ = std::make_unique<HitMotionSystem>();
		hitMotionSystem_->Initialize(this, effect);

		// オブジェクト状態生存
		objectComponent_->GetObjectStateFlags().isAlive = true;

		// 戦闘
		attackController_ = std::make_unique<AttackController>();
		attackController_->Initialize(entityManager, globalVariables, GetCharacterParameterComponent(), this);

		//死亡システム
		deathSystem_ = std::make_unique<DeathSystem>();
		deathSystem_->SetOwner(this);

		// コンテキストシステム
		contextSystem_ = std::make_unique<CharacterContextSystem>();
		contextSystem_->Initialize(this, inputSystem);

		// ステートマシーン初期化
		InitStateMachine();

		// 敵AIシステム
		attackSystem_ = std::make_unique<EnemyAttackSystem>();
		attackSystem_->Initialize(stateMachine_.get());


		// 丸影用オブジェクトコンポーネント初期化
		InitShadowObjectComponent(charaName);
	}

	void BaseEnemy::BaseUpdate() {

		if (objectComponent_ == nullptr) { return; }
		assert(this);

		CharacterContext ctx = contextSystem_->CreateContext(this, GetTime());

		// 保存項目更新(キャラクター全体)
		UpdateBaseGetValue();
		// 保存項目更新(敵全体)
		UpdateBaseEnemyGetValue();


		//　攻撃更新
		attackController_->Update(ctx);
		// 応答システム
		hitMotionSystem_->Update(ctx.dt);
		// キャラクターパラメーター更新
		parameterComponent_->Update();
		// 移動コンポーネント更新
		moveComponent_->Update(objectComponent_->GetWorldTransform(), *objectComponent_->GetRigidBodyComponent(), ctx);

		// コライダのワールドトランスフォーム更新
		worldCollider_->Update();
		// コライダーコンポーネント更新
		objectComponent_->GetColliderComponent()->UpdateAll(*worldCollider_.get());

		objectComponentShadow_->GetWorldTransform().translate_.x = GetWorldTransform().translate_.x;
		objectComponentShadow_->GetWorldTransform().translate_.z = GetWorldTransform().translate_.z;
		objectComponentShadow_->GetWorldTransform().translate_.y = 0.02f;

		// 死亡システム更新
		deathSystem_->Update(ctx.dt);
		// ステート
		stateMachine_->Update(ctx);


		objectComponentShadow_->Update();
	}

	void BaseEnemy::InitializeBaseEnemyAddItem() {
		AddItem("後退スピード", globalData_.retreatSpeed);
		AddItem("攻撃猶予時間", globalData_.attackTimer);
		AddItem("回転速度", globalData_.turnSpeed);



		globalData_.retreatSpeed = GetValue<float>("後退スピード");
		globalData_.attackTimer = GetValue<float>("攻撃猶予時間");
		globalData_.turnSpeed = GetValue<float>("回転速度");

	}
	// 更新保存項目
	void BaseEnemy::UpdateBaseEnemyGetValue() {
		globalData_.retreatSpeed = GetValue<float>("後退スピード");
		globalData_.attackTimer = GetValue<float>("攻撃猶予時間");
		globalData_.turnSpeed = GetValue<float>("回転速度");
	}

#pragma endregion 

	EnemyAttackSystem* BaseEnemy::GetEnemyAttackSystem() const {
		return attackSystem_.get();
	}

	const BaseCharacter* BaseEnemy::GetTarget() {
		return GetAttackController()->GeyLockOnSysutem()->GetTarget();
	}

	void BaseEnemy::SetTargetCharacters(BaseCharacter* target) {
		GetAttackController()->GeyLockOnSysutem()->SetTargets({ target });
	}

}
