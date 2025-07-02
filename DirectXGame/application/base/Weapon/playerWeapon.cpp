#include "playerWeapon.h"
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Player.h"

void PlayerWeapon::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	objectBase_ = entity3DManager->CreateObject3D("PlayerWeapon", Object3d::ObjectModelType::kNormal, {}, camera);
	objectBase_->SetIsDraw(false);
	objectBase_->SetModel("Sword.obj");
	objectBase_->worldtransform_.scale_ = { 1.25f,1.25f ,1.25f };
	objectBase_->InitColliderComponent(); // コライダーコンポーネントの初期化
	objectBase_->SetIsUpdateColliderComponent(false); // コライダーの更新は手動で行うため、Object3d内での更新無効化
	

	auto obbCollider_ = std::make_unique<OBBCollider>();
	obbCollider_->obb.size = { 0.5f,2.0f,1.0f };
	obbCollider_->tag = CollisionTag::PlayerAttack;
	obbCollider_->layer = CollisionLayer::PlayerAttack;
	obbCollider_->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	weaponColliderId_ = objectBase_->GetColliderComponent()->GetNextId();
	objectBase_->GetColliderComponent()->AddCollider(std::move(obbCollider_));
	
	auto obbCollider2_ = std::make_unique<OBBCollider>();
	obbCollider2_->obb.size = { 0.5f,2.5f,1.0f };
	obbCollider2_->tag = CollisionTag::PlayerAttack;
	obbCollider2_->layer = CollisionLayer::PlayerAttack;
	obbCollider2_->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	weaponColliderId2_ = objectBase_->GetColliderComponent()->GetNextId();
	objectBase_->GetColliderComponent()->AddCollider(std::move(obbCollider2_));
	

	objectBase_->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		if (!other || other->tag != CollisionTag::Enemy) return;

		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());
		
		if (!enemy) return;

		const uint32_t otherId = otherComponent->GetUniqueId();
		const float nowTime = MyGame::NowTime();

		Player* player = static_cast<Player*>(basePlayer_);

		if (player->GetAttackType() == Player::AttackTypePlay::kJump) {
			if (objectBase_->GetColliderComponent()->contactRecord_.CheckHistory(otherId, nowTime,0.1f)) {
				return; // クールタイム中のため無視
			}
		}
		else {
			if (objectBase_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
				return; // クールタイム中のため無視
			}
		}
		

		objectBase_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		if (self->id == weaponColliderId_) {
			enemy->AddDamage(20);
			enemy->hitStop(0.2f);
		}else if(self->id == weaponColliderId2_) {
			enemy->AddDamage(10);
			enemy->hitStop(0.1f);
		}
		enemy->SetHit();
		enemy->Emit();
		

		player->AddHit();
		player->AddSP();
		player->SetHitTime();
		};

	
	
	colliderWorld_.Initialize();
	colliderWorld_.parent_ = &objectBase_->worldtransform_;
	colliderWorld_.translate_.z = 0.5f; // 武器の位置調整
	colliderWorld_.translate_.y = 3.0f; // 武器の位置調整

	colliderWorld2_.Initialize();
	colliderWorld2_.parent_ = &colliderWorld_;
	colliderWorld2_.translate_.y = 4.0f; // 武器の位置調整
}

void PlayerWeapon::Update()
{
	colliderWorld_.Update();
	colliderWorld2_.Update();

	objectBase_->GetColliderComponent()->UpdateByID(colliderWorld_, weaponColliderId_);
	objectBase_->GetColliderComponent()->UpdateByID(colliderWorld2_, weaponColliderId2_);

}

void PlayerWeapon::Draw2D()
{
}

void PlayerWeapon::DrawEffect()
{
}

void PlayerWeapon::SetCharacter(BaseCharacter* character)
{
	basePlayer_ = static_cast<BasePlayer*>(character);
}
