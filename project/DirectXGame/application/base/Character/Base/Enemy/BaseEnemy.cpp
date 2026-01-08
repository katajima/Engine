#include "BaseEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"

Vector3 BaseEnemy::GetTargetPos()
{
	// ターゲット位置
	return player_->GetObjectComponent()->GetWorldTransform().GetWorldPosition();
}

float BaseEnemy::GetTargetDistance()
{
	// ターゲットとの距離
	return GetObjectComponent()->GetWorldTransform().GetWorldPosition().DistanceXZ(player_->GetObjectComponent()->GetWorldTransform().GetWorldPosition());
}

void BaseEnemy::DirectionMoveVelocity(float speed)
{
	// 距離
	Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
	
	// 回転設定
	Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);

	// Y軸周り角度
	GetWorldTransform().rotate_.y = rotate.y;

	// 速度設定
	Velocity() = dire * speed;
}

void BaseEnemy::DirectionMove(float speed)
{
	// 距離設定
	Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
	// 回転設定
	Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);

	// Y軸周り角度
	GetWorldTransform().rotate_.y = rotate.y;
}

Vector3 BaseEnemy::TargetDirection()
{
	// ターゲットの方向
	return Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
}

void BaseEnemy::Initialize2D()
{

	// ロックオン
	icon_lockOn = std::make_unique<Engine::Sprite>();
	icon_lockOn->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/LockOnW.png", false);
	icon_lockOn->SetSize(0.10f);		// サイズ指定
	icon_lockOn->SetColor({ 1,0,1,1 });	// 色指定
	icon_lockOn->SetPosition({ -100,650 });	// 位置指定
	icon_lockOn->SetAnchorPoint({ 0.5f,0.5f });	// アンカーポイント設定
}

void BaseEnemy::BaseInitialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, 
	Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
	const std::string& modelName, const std::string& charaName){

	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	// オブジェクトインスタンシング初期化
	objectComponent_->InitializeInstancing(entity3DManager_, globalVariables_, charaName + std::to_string(id_), modelName, "", true, true, this);
	objectComponent_->GetColliderComponent()->SetHitReceiver(this);	// インターフェース設定	


	// 移動コンポーネント初期化
	moveComponent_ = std::make_unique<MovementComponent>();
	moveComponent_->UseGlobal(false);
	moveComponent_->Initialize(globalVariables_, MovementComponent::ControlType::Manual, "_" + charaName);
	moveComponent_->SetControlType(MovementComponent::ControlType::Auto);

	// 保存項目追加
	CreateGroup(charaName);

	// SphereColliderを追加
	auto sphere = std::make_unique<Engine::SphereCollider>();
	sphere->Enable();					// コライダ有効
	sphere->tag = CollisionTag::Enemy;	// タグ設定
	sphere->layer = CollisionLayer::Enemy;// レイヤー設定
	sphere->radius = 3.0f; // 半径を適宜設定
	GetColliderComponent()->AddCollider(std::move(sphere));	// コライダ追加

	// コールバック登録（例：プレイヤーと衝突したらダメージ）
	GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		// プレイヤーかチェック
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		//  敵同士の衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Enemy, self, other);

		// プレイヤーとの衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Player, self, other);

		// 壁との衝突応答
		responseSystem_->GetHitResponse()->Hit(CollisionTag::Wall, self, other);

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
	responseSystem_->Initialize(&GetCharacterParameterComponent(), objectComponent_.get());
	responseSystem_->GetHitResponse()->SetOwner(&objectComponent_->GetWorldTransform());

	// オブジェクト状態生存
	objectComponent_->GetObjectStateFlags().isAlive = true;

	// 戦闘中の倍率・軽減率を扱う
	attackController_ = std::make_unique<AttackController>();
	attackController_->Initialize(entity3DManager, globalVariables_, &characterParameterComponent_, this);


	// ステートマシーン初期化
	InitStateMachine();
}

void BaseEnemy::BaseUpdate(){

	if (GetObjectComponent() == nullptr) { return; }
	assert(this);

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
		moveComponent_->Update(GetTime(), GetObjectComponent()->GetWorldTransform(), *GetObjectComponent()->GetRigidBodyComponent(), nullptr);
		// 視野
		visionComponent_->Update(GetTime(), GetObjectComponent()->GetWorldTransform().GetWorldPosition(), moveComponent_->GetDirection(), player_->GetWorldTransform().translate_);
		// 応答システム
		responseSystem_->Update(GetTime());
	}
}
