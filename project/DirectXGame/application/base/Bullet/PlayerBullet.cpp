#include "PlayerBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"

// 初期化
void PlayerBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) {
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;

	object_ = entity3DManager->CreateObject3D("playerStanBullet", ObjectModelType::kNormal, position, camera);
	object_->SetModel("AnimatedCube.gltf");
	object_->GetWorldTransform().scale_ = { 0.2f,0.2f,0.2f };
	object_->UseTrailEffect("resources/Texture/Image.png", 0.15f, { 1.0f,1.0f,1.0f,1.0f }, { 0,0.2f,0 }, { 0,-0.2f,0 });
	object_->Update();
	object_->isEmitTrailEffect = false;
	object_->InitColliderComponent();
	object_->SetIsDraw(true);

	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;
	sphere->layer = CollisionLayer::PlayerAttack;
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	sphere->radius = 2.0f; // 半径を適宜設定
	sphere->Enable();
	object_->GetColliderComponent()->AddCollider(std::move(sphere));


	parameter_.damege = info_.damage;


	

	velocity_ = Normalize(info_.targetPos - object_->GetWorldTransform().GetWorldPosition()) * info_.speed;
	

	// 衝突時のコールバック登録
	object_->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent || other->tag != CollisionTag::Enemy) return;
		if (isAlive_ == false) return;

		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		uint32_t otherId = otherComponent->GetUniqueId();




		float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

		if (object_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		object_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		enemy->AddDamage(parameter_.damege);
		if (enemy->GetCharacterParameterComponent().GetHP() > 0) {
			//enemy->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
		enemy->Emit();
		}
		hitDelete_ = true;
		};

	timer_ = 0.0f;
	deleTimer_ = 5.0f;
	isAlive_ = true;

};

// 毎フレーム更新
void PlayerBullet::Update() {
	if (timer_ != 0.0f) {
		object_->isEmitTrailEffect = true;
	}

	timer_ += GetTimer();
	if (object_->GetWorldTransform().translate_.y <= -3.0f) {
		velocity_ = 0.0f;
		object_->GetWorldTransform().translate_.y = -3.0f;
		hitDelete_ = true;
	}
	else {
		object_->GetWorldTransform().translate_ += velocity_ * GetTimer();
	}

	if (deleTimer_ <= timer_ || hitDelete_) {
		Final();
	}
};

// 描画
void PlayerBullet::Draw() {
};
//描画エフェクト
void PlayerBullet::DrawP() {
};
//描画2d
void PlayerBullet::Draw2D() {

}
void PlayerBullet::Final()
{
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
;