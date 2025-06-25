#include "playerWeapon.h"
#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"
#include "DirectXGame/application/base/Player/Base/BasePlayer.h"

void playerWeapon::Initialize(Entity3DManager* entity3DManager, Camera* camera)
{
	// コライダコンポーネント追加
	colliderComponent_ = std::make_unique<ColliderComponent>();
	colliderComponent_->SetOwner(colliderComponent_.get());
	colliderComponent_->SetLineCommon(entity3DManager->Get3DLineCommon());
	colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());

	auto obbCollider_ = std::make_unique<OBBCollider>();
	obbCollider_->obb.size = { 0.5f,3,1 };
	obbCollider_->tag = CollisionTag::PlayerAttack;
	obbCollider_->layer = CollisionLayer::PlayerAttack;
	obbCollider_->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	colliderComponent_->AddCollider(std::move(obbCollider_));

	//auto sphere = std::make_unique<SphereCollider>();
	//sphere->tag = CollisionTag::PlayerAttack;
	//sphere->layer = CollisionLayer::Enemy;
	//sphere->radius = 3.0f; // 半径を適宜設定
	//colliderComponent_->AddCollider(std::move(sphere));

	colliderComponent_->onHitCallback = [this](Collider* self, Collider* other) {
		if (!other || other->tag != CollisionTag::Enemy) return;

		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());
		enemy->GetSerialNumber(); // シリアル番号を取得しておく（デバッグ用）

		if (!enemy) return;

		const uint32_t otherId = otherComponent->GetUniqueId();
		const float nowTime = MyGame::NowTime();

		if (colliderComponent_->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		colliderComponent_->contactRecord_.AddHistory(otherId, nowTime);

		enemy->AddDamage(10);
		enemy->SetHit();
		enemy->Emit();
		enemy->hitStop(0.1f);

		player_->AddHit();
		player_->AddSP();
		player_->SetHitTime();
		};

	objectWeapon_ = std::make_unique<Object3d>();
	objectWeapon_->Initialize(entity3DManager);
	objectWeapon_->SetCamera(camera);
	objectWeapon_->SetIsDraw(false);
	objectWeapon_->SetModel("Sword.obj");
	entity3DManager->SetEntity3D(std::move(objectWeapon_));

	//colliderWorld_ = std::make_unique<WorldTransform>();
	colliderWorld_.Initialize();
	colliderWorld_.parent_ = &objectWeapon_->worldtransform_;
	colliderWorld_.translate_.y = 5.0f; // 武器の位置調整
}

void playerWeapon::Update()
{
	colliderWorld_.Update();
	colliderComponent_->UpdateAll(colliderWorld_);
	//objectWeapon_->Update();
}

void playerWeapon::Draw()
{
	//objectWeapon_.Draw();
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
