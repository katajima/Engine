#include "playerWeapon.h"
#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"

void playerWeapon::Initialize(Entity3DManager* entity3DManager, Camera* camera)
{
	objectWeapon_ = entity3DManager->CreateObject3D("PlayerWeapon", Object3d::ObjectType::kNormal, {}, camera);
	objectWeapon_->SetIsDraw(false);
	objectWeapon_->SetModel("Sword.obj");
	objectWeapon_->worldtransform_.scale_ = { 1.25f,1.25f ,1.25f };
	objectWeapon_->InitColliderComponent(); // コライダーコンポーネントの初期化
	objectWeapon_->SetIsUpdateColliderComponent(false); // コライダーの更新は手動で行うため、Object3d内での更新無効化
	

	auto obbCollider_ = std::make_unique<OBBCollider>();
	obbCollider_->obb.size = { 0.5f,2.0f,1.0f };
	obbCollider_->tag = CollisionTag::PlayerAttack;
	obbCollider_->layer = CollisionLayer::PlayerAttack;
	obbCollider_->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	weaponColliderId_ = objectWeapon_->GetColliderComponent()->GetNextId();
	objectWeapon_->GetColliderComponent()->AddCollider(std::move(obbCollider_));
	
	auto obbCollider2_ = std::make_unique<OBBCollider>();
	obbCollider2_->obb.size = { 0.5f,2.5f,1.0f };
	obbCollider2_->tag = CollisionTag::PlayerAttack;
	obbCollider2_->layer = CollisionLayer::PlayerAttack;
	obbCollider2_->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	weaponColliderId2_ = objectWeapon_->GetColliderComponent()->GetNextId();
	objectWeapon_->GetColliderComponent()->AddCollider(std::move(obbCollider2_));
	

	objectWeapon_->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		if (!other || other->tag != CollisionTag::Enemy) return;

		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());
		
		if (!enemy) return;

		const uint32_t otherId = otherComponent->GetUniqueId();
		const float nowTime = MyGame::NowTime();

		if (player_->GetAttackType() == Player::AttackTypePlay::kJump) {
			if (objectWeapon_->GetColliderComponent()->contactRecord_.CheckHistory(otherId, nowTime,0.1f)) {
				return; // クールタイム中のため無視
			}
		}
		else {
			if (objectWeapon_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
				return; // クールタイム中のため無視
			}
		}
		

		objectWeapon_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		if (self->id == weaponColliderId_) {
			enemy->AddDamage(20);
			enemy->hitStop(0.2f);
		}else if(self->id == weaponColliderId2_) {
			enemy->AddDamage(10);
			enemy->hitStop(0.1f);
		}
		enemy->SetHit();
		enemy->Emit();
		

		player_->AddHit();
		player_->AddSP();
		player_->SetHitTime();
		};

	
	
	colliderWorld_.Initialize();
	colliderWorld_.parent_ = &objectWeapon_->worldtransform_;
	colliderWorld_.translate_.z = 0.5f; // 武器の位置調整
	colliderWorld_.translate_.y = 3.0f; // 武器の位置調整

	colliderWorld2_.Initialize();
	colliderWorld2_.parent_ = &colliderWorld_;
	colliderWorld2_.translate_.y = 4.0f; // 武器の位置調整
}

void playerWeapon::Update()
{
	colliderWorld_.Update();
	colliderWorld2_.Update();

	objectWeapon_->GetColliderComponent()->UpdateByID(colliderWorld_, weaponColliderId_);
	objectWeapon_->GetColliderComponent()->UpdateByID(colliderWorld2_, weaponColliderId2_);

}

void playerWeapon::Draw()
{
}



void playerWeapon::ContactRecordClear()
{
	contactRecord_.Clear();
}

void playerWeapon::SetPlayer(Player* player)
{
	player_ = player;
}

void playerWeapon::SetPlayer(BasePlayer* player)
{
	basePlayer_ = player;
}
