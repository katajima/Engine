#include "BossEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"

// 初期化
void BossEnemy::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera){
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	CreateGroup("BossEnemy");

	objectBase_ = entity3DManager_->CreateObject3D("enemy" + std::to_string(id_), ObjectModelType::kNormal, {}, camera);
	objectBase_->SetModel("enemy2.obj");
	objectBase_->GetWorldTransform().translate_ = position;
	objectBase_->GetWorldTransform().scale_ = { 1.7f,1.7f,1.7f };
	objectBase_->InitColliderComponent();
	GetColliderComponent()->SetHitReceiver(this);
	InitMoveComponent();
	objectBase_->InitRigidBodyComponent();

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
					objectBase_->GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {
					// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					objectBase_->GetWorldTransform().translate_ += pushVec * 0.5f;
				}

				objectBase_->GetWorldTransform().Update();
			}
		}

		if (other->tag == CollisionTag::Player) {
			// ここにダメージ処理などを書く
			std::cout << "敵がプレイヤーに当たった！" << std::endl;

			Vector3 pushVec;
			if (self->ResolveCollision(*other, pushVec)) {
				pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					objectBase_->GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {
					// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					objectBase_->GetWorldTransform().translate_ += pushVec * 0.5f;
				}

				objectBase_->GetWorldTransform().Update();
			}
		}
		if (other->tag == CollisionTag::Wall) {
			Vector3 pushVec;
			if (self->ResolveCollision(*other, pushVec)) {
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					objectBase_->GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {
					// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					objectBase_->GetWorldTransform().translate_ += pushVec * 0.5f;
				}
				//acceleration_.y = 0;
				Velocity().y = 0;
				//velocity_.y = 0;
				//flags_.isGrounded = true;
				objectBase_->GetWorldTransform().Update();
			}
		}
		if (other->tag == CollisionTag::PlayerAttack) {
			ChangeState("Move");
		}


		};

	// 視野
	visionComponent_ = std::make_unique<VisionComponent>();
	visionComponent_->SetAlertView(120.0f, 100.0f);
	visionComponent_->SetCombatView(90.0f, 100.0f);

	// ヒットモーション
	hitMotionComponent_ = std::make_unique<HitMotionComponent>();

	hitMotionComponent_->Init(0.1f, { 2.5f,2.2f,2.5f });

};

// 毎フレーム更新
void BossEnemy::Update()
{


};


void BossEnemy::DrawEffect()
{
};
//
void BossEnemy::Draw2D()
{

}

void BossEnemy::Emit()
{
};
// 移動
void BossEnemy::Move()
{
};

void BossEnemy::Jump()
{
};
