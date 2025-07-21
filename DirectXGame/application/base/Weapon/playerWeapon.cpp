#include "playerWeapon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"

void PlayerWeapon::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	input_ = input;

	objectBase_ = entity3DManager->CreateObject3D("PlayerWeapon", Object3d::ObjectModelType::kNormal, {}, camera);
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

		if (GetAttackInput().GetAttackTypePlay() == AttackTypePlay::kJump) {
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

		enemy->GetHitMotionComponent()->SetIsHit(true);
		data_.knockbackData.normal = player->GetMoveComponent()->GetDirection();
		if (self->id == weaponColliderId_) {
			enemy->GetHitMotionComponent()->SetKnockbackData(data_.knockbackData);
			enemy->GetHitMotionComponent()->SethitStopTime(1.1f);
			enemy->AddDamage(GetDamage() * 2);
			enemy->GetHitMotionComponent()->SetKnockbackTime(0.5f);
			enemy->GetHitMotionComponent()->SetIsKnockback(true);



		}else if(self->id == weaponColliderId2_) {
			enemy->GetHitMotionComponent()->SetKnockbackData(data_.knockbackData);
			enemy->GetHitMotionComponent()->SethitStopTime(1.1f);
			enemy->AddDamage(GetDamage());
			enemy->GetHitMotionComponent()->SetKnockbackTime(0.5f);
			enemy->GetHitMotionComponent()->SetIsKnockback(true);
			
		}
		enemy->Emit();
		
		player->AddHit();
		player->AddSpGauge(1);
		player->SetHitTime();
		};

	
	data_.damage = 10; // 武器のダメージ設定
	data_.knockbackData.power = 25.5f; // ノックバックの力設定
	data_.knockbackData.yPower = 34.5f; // Y方向のノックバックの力設定
	SetTime(0.1f, 0.3f, 0.1f); // 武器のアニメーション時間設定


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


void PlayerWeapon::AttackTypeInit(int comboIndex)
{
	ColliderHistoryClear();
	if (GetAttackInput().GetTypeRequest()) {
		// ふるまいを変更する
		GetAttackInput().ChangeRequest();

		GetComboMotionData().ResetTime();

		// 各ふるまいごとの初期化を実行
		switch (GetAttackInput().GetAttackTypePlay())
		{
		case AttackTypePlay::kNormal:

			if (comboIndex == 0) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
			}
			if (comboIndex == 1) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
			}
			if (comboIndex == 2) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,-90 });
			}
			if (comboIndex == 3) {
				SetTime(0.1f, 0.2f, 0.01f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
			}
			break;
		case AttackTypePlay::kJump:
			if (comboIndex == 0) {
				SetTime(0.0f, 0.4f, 0.1f);
				GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
			}
			break;
		}
		// ふるまいリクエストリセット
		GetAttackInput().ResetRequest();
	}
}

void PlayerWeapon::AttackUpdate(float deltaTime, WorldTransform& worldTransform)
{
	GetTimer().Update(deltaTime);

	GetObject3D()->SetIsDraw(true);
	switch (GetAttackInput().GetAttackTypePlay())
	{
	case AttackTypePlay::kNormal:
		if (GetComboData().GetCurrentComboCount() == 0) {
			if (GetComboMotionData().isStartup) {
				SetMovementSpeedMultiplier(0.01f);
				GetWorldTransform().rotate_.x += DegreesToRadians(1 * 60) * deltaTime;
			}
			if (GetComboMotionData().isAttackAnimation) {
				SetMovementSpeedMultiplier(0.1f);
				GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;
			}
			if (GetComboMotionData().isRecovery) {
				SetMovementSpeedMultiplier(1.05f);
				GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;
			}
		}if (GetComboData().GetCurrentComboCount() == 1) {
			if (GetTimer().t >= GetComboMotionData().startupTime) {
				SetMovementSpeedMultiplier(0.3f);
				GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;
			}
		}if (GetComboData().GetCurrentComboCount() == 2) {
			if (GetTimer().t >= GetComboMotionData().startupTime) {
				SetMovementSpeedMultiplier(0.4f);
				GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * deltaTime;
			}
		}if (GetComboData().GetCurrentComboCount() == 3) {
			if (GetTimer().t >= GetComboMotionData().startupTime) {
				SetMovementSpeedMultiplier(1.5f);
				GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * deltaTime;
			}
		}


		if (GetTimer().t <= 5.0f / 60) {

			Vector3 move(0, 0, GetMovementSpeedMultiplier());
			// 速度ベクトルを自機の向きに合わせて回転させる
			move = TransformNormal(move, worldTransform.worldMat_);

			worldTransform.translate_ += move;
		}
		break;
	case AttackTypePlay::kJump:
		if (GetTimer().t >= 1.0f / 60) {
			GetWorldTransform().rotate_.x += DegreesToRadians(16 * 180) * deltaTime;
			SetMovementSpeedMultiplier(0.2f);
			Vector3 move(0, 0, GetMovementSpeedMultiplier());
			// 速度ベクトルを自機の向きに合わせて回転させる
			move = TransformNormal(move, worldTransform.worldMat_);
			worldTransform.translate_ += move;
		}
		break;
	}
}
