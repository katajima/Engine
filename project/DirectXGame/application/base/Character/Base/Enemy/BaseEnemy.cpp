#include "BaseEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"

namespace Character {

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

	void BaseEnemy::Initialize2D()
	{

		// ロックオン
		icon_lockOn = std::make_unique<Engine::Sprite>();
		icon_lockOn->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/icon/LockOnW.dds", false);
		icon_lockOn->SetSize(0.10f);		// サイズ指定
		icon_lockOn->SetColor({ 1,0,1,1 });	// 色指定
		icon_lockOn->SetPosition({ -100,650 });	// 位置指定
		icon_lockOn->SetAnchorPoint({ 0.5f,0.5f });	// アンカーポイント設定
	}

	void BaseEnemy::InitShadowObjectComponent(const std::string& charaName)
	{
		// オブジェクトコンポーネント追加
		objectComponentShadow_ = std::make_unique<ObjectComponent>();
		// オブジェクトインスタンシング初期化
		objectComponentShadow_->InitializeInstancing(entityManager, globalVariables, charaName + std::to_string(id_), "plane.obj", "resources/Texture/smoke/no4.dds",
			false, false, this, Engine::Object3dInstansManager::TransparencyType::kYes);
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
			, Engine::Object3dInstansManager::TransparencyType::kNo);
		objectComponent_->GetColliderComponent()->SetHitReceiver(this);	// インターフェース設定	


		// キャラクターのパラメータコンポーネントを生成
		parameterComponent_ = std::make_unique<Character::ParameterComponent>();
		parameterComponent_->Initialize();

		// 移動コンポーネント初期化
		moveComponent_ = std::make_unique<MovementComponent>();
		moveComponent_->UseGlobal(false);
		moveComponent_->Initialize(this,nullptr,globalVariables, MovementComponent::ControlType::Manual, "_" + charaName);
		moveComponent_->SetControlType(MovementComponent::ControlType::Auto);

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
		sphere->layer = CollisionLayer::Enemy;// レイヤー設定
		sphere->radius = colliderRadius; // 半径を適宜設定
		//sphere->isDebugLine = true;
		GetColliderComponent()->AddCollider(std::move(sphere));	// コライダ追加

		// コールバック登録（例：プレイヤーと衝突したらダメージ）
		GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
			// プレイヤーかチェック
			auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
			if (!otherComponent) return;

			//  敵同士の衝突応答
			responseSystem_->GetHitResponse()->Hit(CollisionTag::Enemy, self, other);

			// 壁との衝突応答
			responseSystem_->GetHitResponse()->Hit(CollisionTag::Wall, self, other);

			};


		// 視野
		visionComponent_ = std::make_unique<VisionComponent>();
		visionComponent_->SetAlertView(120.0f, 100.0f);
		visionComponent_->SetCombatView(90.0f, 100.0f);
		visionComponent_->SetLineCommon(entityManager->Get3DLineCommon());
		visionComponent_->raycastFunc = [this](Vector3 origin, Vector3 dir, float maxDist)-> bool {return false; };


		// 攻撃応答システムクラス初期化
		responseSystem_ = std::make_unique<ResponseSystem>();
		responseSystem_->Initialize(GetCharacterParameterComponent(), objectComponent_.get());
		responseSystem_->GetHitResponse()->SetOwner(moveComponent_->GetResponseMoveSystem());
		// オブジェクト状態生存
		objectComponent_->GetObjectStateFlags().isAlive = true;

		// 戦闘
		attackController_ = std::make_unique<AttackController>();
		attackController_->Initialize(entityManager, globalVariables, GetCharacterParameterComponent(), this);

		// コンテキストシステム
		contextSystem_ = std::make_unique<CharacterContextSystem>();
		contextSystem_->Initialize(this, inputSystem);

		// 丸影用オブジェクトコンポーネント初期化
		InitShadowObjectComponent(charaName);

		// ステートマシーン初期化
		InitStateMachine();
	}

	void BaseEnemy::BaseUpdate() {

		if (GetObjectComponent() == nullptr) { return; }
		assert(this);

		CharacterContext ctx = contextSystem_->CreateContext(GetTime());

		// 保存項目更新(キャラクター全体)
		UpdateBaseGetValue();
		// 保存項目更新(敵全体)
		UpdateBaseEnemyGetValue();

		// ステート
		stateMachine_->Update();

		// HPが0以下なら
		if (GetHP() <= 0) {
			GetObjectComponent()->GetObjectStateFlags().isLockonTarget = false;
			GetObjectComponent()->GetObjectStateFlags().isAlive = false;

			objectComponentShadow_->GetWorldTransform().scale_ = {};
		}
		else {
			// 移動コンポーネント更新
			moveComponent_->Update(GetObjectComponent()->GetWorldTransform(), *GetObjectComponent()->GetRigidBodyComponent(),ctx);
			// 応答システム
			responseSystem_->Update(GetTime());


			objectComponentShadow_->GetWorldTransform().translate_.x = GetWorldTransform().translate_.x;
			objectComponentShadow_->GetWorldTransform().translate_.z = GetWorldTransform().translate_.z;
			objectComponentShadow_->GetWorldTransform().translate_.y = -3.0f;
		}
	
		objectComponentShadow_->Update();
	}
	
#pragma endregion 


	const BaseCharacter* BaseEnemy::GetTarget() {
		return GetAttackController()->GeyLockOnSysutem()->GetTarget();
	}

	void BaseEnemy::SetTargetCharacters(BaseCharacter* target) {
		GetAttackController()->GeyLockOnSysutem()->SetTargets({target});
	}
	
}
