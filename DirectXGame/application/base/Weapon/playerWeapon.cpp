#include "playerWeapon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"

void PlayerWeapon::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	input_ = input;

	objectBase_ = entity3DManager->CreateObject3D("PlayerWeapon", ObjectModelType::kNormal, {}, camera);
	objectBase_->SetIsDraw(false);
	objectBase_->SetModel("Sword.obj");
	objectBase_->GetWorldTransform().scale_ = { 1.25f,1.25f ,1.25f };
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

		Player* player = static_cast<Player*>(character);

		//if (GetAttackInput().GetAttackTypePlay() == AttackTypePlay::kJump) {
		//	if (objectBase_->GetColliderComponent()->contactRecord_.CheckHistory(otherId, nowTime,0.1f)) {
		//		return; // クールタイム中のため無視
		//	}
		//}
		//else {
			if (objectBase_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
				return; // クールタイム中のため無視
			}
		//}
		

		objectBase_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		enemy->GetHitMotionComponent()->SetIsHit(true);

		



		comboData_.knockbackData.normal = player->GetMoveComponent()->GetDirection();
		if (self->id == weaponColliderId_) {
			enemy->GetHitMotionComponent()->SetKnockbackData(comboData_.knockbackData);
			enemy->GetHitMotionComponent()->SethitStopTime(1.1f);
			enemy->AddDamage(comboData_.damage * 2);
			enemy->GetHitMotionComponent()->SetKnockbackTime(0.5f);
			enemy->GetHitMotionComponent()->SetIsKnockback(true);
		}else if(self->id == weaponColliderId2_) {
			enemy->GetHitMotionComponent()->SetKnockbackData(comboData_.knockbackData);
			enemy->GetHitMotionComponent()->SethitStopTime(1.1f);
			enemy->AddDamage(comboData_.damage);
			enemy->GetHitMotionComponent()->SetKnockbackTime(0.5f);
			enemy->GetHitMotionComponent()->SetIsKnockback(true);
			
		}
		enemy->Emit();
		
		player->AddHit();
		player->AddSpGauge(1);
		player->SetHitTime();
		};

	ComboData data;
	data.damage = 10;
	data.knockbackData.power = 30;
	data.knockbackData.yPower = 30;
	data.movementSpeedMultiplier = 1.0f;
	data.mpCost = 0.0f;
	data.staminaCost = 1.0f;


	
	attack1  = std::make_shared<ComboNodeState>("Attack1", data);
	data.damage = 11;
	attack2 = std::make_shared<ComboNodeState>("Attack2", data);
	data.damage = 12;
	attack3 = std::make_shared<ComboNodeState>("Attack3", data);
	//data.damage = 30;
	//heavy1   = std::make_shared<ComboNodeState>("Heavy1", data);

	
	AddComboNode("Attack1", attack1);
	AddComboNode("Attack2", attack2);
	AddComboNode("Attack3", attack3);

	ConnectCombo("Attack1",AttackInput::Light, "Attack2");
	ConnectCombo("Attack2",AttackInput::Light, "Attack3");

	//attack1->SetNextState(AttackInput::Light, attack2);
	//attack2->SetNextState(AttackInput::Light, attack3);
	//attack2->SetNextState(AttackInput::Heavy, heavy1);

	
	//comboStateMachine_->SetRoot(comboRoot);


	colliderWorld_.Initialize();
	colliderWorld_.parent_ = &objectBase_->GetWorldTransform();
	colliderWorld_.translate_.z = 0.5f; // 武器の位置調整
	colliderWorld_.translate_.y = 3.0f; // 武器の位置調整

	colliderWorld2_.Initialize();
	colliderWorld2_.parent_ = &colliderWorld_;
	colliderWorld2_.translate_.y = 4.0f; // 武器の位置調整
}

void PlayerWeapon::Update()
{
	objectBase_->UpdateWorldTransform();
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

