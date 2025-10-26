#include "PlayerStanBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"


// 初期化
void PlayerStanBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) {
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	
	object_ = entity3DManager->CreateObject3D("playerStanBullet", ObjectModelType::kNormal, position, camera);
	object_->SetModel("AnimatedCube.gltf");
	object_->GetWorldTransform().scale_ = { 2,2,2 };
	object_->Update();
	object_->InitColliderComponent();
	object_->SetIsDraw(true);
	
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;
	sphere->layer = CollisionLayer::PlayerAttack;
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	sphere->radius = 20.0f; // 半径を適宜設定
	sphere->Disable(); // 初期状態では無効化
	object_->GetColliderComponent()->AddCollider(std::move(sphere));


	parameter_.damege = 0;
	
	
	
	velocity_ = player_->GetObjectComponent()->GetObject3D()->ObjectDirection();
	velocity_ *= 30.0f;
	velocity_.y = 100.0f;
	
	
	// 衝突時のコールバック登録
	object_->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent || other->tag != CollisionTag::Enemy) return;
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		uint32_t otherId = otherComponent->GetUniqueId();




		float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

		if (object_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		object_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		enemy->AddDamage(parameter_.damege);
		enemy->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
		enemy->Emit();
		};

	timer_ = 0.0f;
	deleTimer_ = 0.0f;
	isAlive_ = true;

};

// 毎フレーム更新
void PlayerStanBullet::Update() {
	SphereCollider* coll = object_->GetColliderComponent()->FindColliderById<SphereCollider>(1);

	timer_ += GetTimer();


	

	if (object_->GetWorldPosition().y >= -3.0f) {
		velocity_.y += -9.8f;
		object_->GetWorldTransform().translate_ += velocity_ * GetTimer();
	}

	if (object_->GetWorldTransform().translate_.y <= -3.0f) {
		velocity_ = 0.0f;
		object_->GetWorldTransform().translate_.y = -3.0f;
	}
	

	

	if (explodingtimer_ <= timer_) {
		coll->enabled = true;
		parameter_.damege = 1.0f;
		isEffectPlay_ = true;
		isAlive_ = false;
		object_->SetIsDraw(false);
		deleTimer_ += GetTimer();
		if (deleTimer_ >= 0.1f) {
			isEffectPlay_ = false;
			object_->IsDelete();
			object_->GetColliderComponent()->ClearColliders();
		}
	}
};

// 描画
void PlayerStanBullet::Draw() {

};
//
void PlayerStanBullet::DrawP() {

};
//
void PlayerStanBullet::Draw2D() {

};