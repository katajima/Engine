#include "BulletPlayer.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "assert.h"

void BulletPlayer::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) 
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	camera_ = camera;
	input_ = input;
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();
	CreateGroup("BulletPlayer");

	// HP設定
	Parameters().HP.Initiaize(1000, 0, 1000, 0);
	HP() = 1000; // 初期HP設定

	Parameters().speed = 20.0f;// 移動速度設定

	// プレイヤー
	objectBase_ = entity3DManager_->CreateObject3D("PlayerBase", Object3d::ObjectModelType::kNormal, position, camera_);
	objectBase_->SetModel("AnimatedCube.gltf");
	objectBase_->Update();
	objectBase_->InitColliderComponent();
	GetColliderComponent()->SetHitReceiver(this);
	InitMoveComponent();

	InitializeBaseAddItem();

	// SphereColliderを追加
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::Player;
	sphere->layer = CollisionLayer::Player;
	sphere->collisionMask = 0xFFFFFFFF;
	sphere->radius = 2.0f; // 半径を適宜設定

	GetColliderComponent()->AddCollider(std::move(sphere));



	// 衝突時のコールバック登録
	GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		uint32_t otherId = otherComponent->GetUniqueId();


		Vector3 pushVec;
		if (other->tag == CollisionTag::Enemy) {
			if (self->ResolveCollision(*other, pushVec)) {
				pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					objectBase_->GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {

				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					objectBase_->GetWorldTransform().translate_ += pushVec * 0.5f;

				}

				objectBase_->Update();
			}
		}
		if (other->tag == CollisionTag::Wall) {
			if (self->ResolveCollision(*other, pushVec)) {
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					objectBase_->GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {

				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					objectBase_->GetWorldTransform().translate_ += pushVec * 0.5f;

				}
				Velocity().y = 0;
				//acceleration_.y = 0;

				objectBase_->Update();
			}
		}
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		if (!enemy) return;
		if (enemy->GetBasicBehavior() == BasicBehavior::kAttack) {
			float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

			if (GetContactRecord().CheckHistory(otherId, nowTime, 1.0f)) {
				return; // クールタイム中のため無視
			}

			GetContactRecord().AddHistory(otherId, nowTime);

			AddDamage(10.0f);
			followCamera_->GetUniqueCamera()->SetShake(0.25f, { 0.1f,0.1f,0.1f });
		}
		};

	// スペシャル攻撃
	special_ = std::make_unique<RangeBombingSpecial>();
	special_->Initialize(entity3DManager, entity2DManager, camera_);
	special_->SetParent(&objectBase_->GetWorldTransform());
	special_->SetInput(input);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
	rengeSp->SetRadius(100);
	rengeSp->SetReticleParent(&objectBase_->GetWorldTransform());
	rengeSp->Set(followCamera_, bulletManager_);

	// 武器
	weapon_ = std::make_unique<PlayerWeapon>();
	weapon_->Initialize(input_, entity3DManager_, nullptr, globalVariables_, {}, camera);
	weapon_->GetObject3D()->GetWorldTransform().parent_ = &objectBase_->GetWorldTransform();
	weapon_->GetObject3D()->GetWorldTransform().translate_ = { 0,0.5f,0.5f };
	weapon_->SetCharacter(this);


	// UI
	ui_ = std::make_unique<PlayerUI>();
	ui_->Initialize(entity2DManager);

	ChangeState("Move");
};
