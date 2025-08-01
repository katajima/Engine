#include "PlayerMissile.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"



#include "DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
//#include "DirectXGame/application/base/Characetr/Player/Player.h"

void PlayerMissile::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	//colliderComponent_ = std::make_unique<ColliderComponent>();
	//colliderComponent_->SetOwner(colliderComponent_.get());
	//colliderComponent_->SetLineCommon(entity3DManager->Get3DLineCommon());
	//colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());

	//auto sphere = std::make_unique<SphereCollider>();
	//sphere->tag = CollisionTag::PlayerAttack;
	//sphere->layer = CollisionLayer::PlayerAttack;
	//sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	//sphere->radius = 3.0f; // 半径を適宜設定
	//colliderComponent_->AddCollider(std::move(sphere));

	//// 衝突時のコールバック登録
	//colliderComponent_->onHitCallback = [this](Collider* self, Collider* other) {
	//	auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
	//	if (!otherComponent || other->tag != CollisionTag::Enemy) return;
	//	BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

	//	uint32_t otherId = otherComponent->GetUniqueId();




	//	float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

	//	if (colliderComponent_->contactRecord_.CheckHistory(otherId)) {
	//		return; // クールタイム中のため無視
	//	}

	//	colliderComponent_->contactRecord_.AddHistory(otherId, nowTime);

	//	enemy->AddDamage(parameter_.damege);
	//	};

	// オブジェクト設定
	object_ = entity3DManager->CreateObject3D("playerbullet", ObjectModelType::kNormal, position, camera);
	object_->SetModel("player_bullet.obj");
	object_->Update();
	// Y軸周り角度(θy)
	object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	// X軸周り角度(θx)
	object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);

	// ダメージ量
	parameter_.damege = 30;
	// 生存フラグ
	isAlive_ = true;
	// 行動フェーズ
	phase_ = 0;


	// 初期地点記録
	str = object_->GetWorldTransform().translate_;

	// ランダムな空中位置を設定
	randPosSky = str + float(rand() % 11 - 5);
	randPosSky.y = max_y;




	// 各演出設定
	ParticleManager* particleManager = entity3DManager->GetEffectManager()->GetParticleManager();

	// ミサイル移動中の煙パーティクルエミッター
	moveSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	InitMoveSmoke(moveSmokeEmitter_.get(), particleManager, "smokePlane01");

	// ミサイル移動中の煙パーティクルエミッター2
	moveSmokeEmitter2_ = std::make_unique <ParticleEmitter>();
	InitMoveSmoke(moveSmokeEmitter2_.get(), particleManager, "smokePlane04");



	Vector3 size = { 2.0f, 2.0f, 2.0f };
	hitEmitter_ = std::make_unique <ParticleEmitter>();
	hitEmitter_->Initialize(particleManager, "missileHit", "missileHit", ParticleData::SpawnType::kPoint);
	hitEmitter_->SetCount(2);
	hitEmitter_->SetLifeTimeMinMax(0.25f, 0.25f);
	hitEmitter_->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	hitEmitter_->SetSizeMinMax(size, size);
	hitEmitter_->SetFrequency(0.00f);
	hitEmitter_->SetParent(object_->GetWorldTransform());
	hitEmitter_->SetUsebillboardRotZ(true);
	hitEmitter_->SetIsAlpha(true);
	hitEmitter_->SetVelocityMinMax({}, {});
	hitEmitter_->SetRotateMinMax({ 0,0,-3.14f }, { 0,0,3.14f });
	hitEmitter_->SetIsEmit(false);


	primitiveCylinder_ = std::make_unique<Primitive>();

	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = 20.0f;
	cylinderParam.innerRadius = 2.0f;
	cylinderParam.outerRadius = 6.0f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;


	//primitiveCylinder_->Initialize<ShapeParameter::Cylinder>(entity3DManager->GetPrimitiveCommon(), Primitive::ShapeType::Cylinder, cylinderParam, "resources/Texture/effect/gradationLine.png");
	//primitiveCylinder_->SetPsoType(Primitive::PsoType::kNoCullRingClamp);

	hitObject_ = std::make_unique<Object3d>();
	hitObject_->Initialize(entity3DManager, ObjectModelType::kPrimitive, ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE);
	//hitObject_->SetPrimitive(primitiveCylinder_.get());
	hitObject_->GetWorldTransform().translate_.z = 50.0f;
	hitObject_->GetWorldTransform().rotate_.y = DegreesToRadians(-90);
	hitObject_->SetName("cylinder");
	hitObject_->SetIsDraw(false);
	hitObject_->SetCamera(camera);

}

void PlayerMissile::Update()
{
	hitEmitter_->SetIsEmit(false);
	// 時間経過でデス
	if (Hit) {
		hitEmitter_->SetIsEmit(true);
		hitEmitter_->Emit();
		isAlive_ = false;
	}

	Vector3 strSmoke = object_->GetWorldTransform().worldMat_.GetWorldPosition();
	Vector3 endSmoke = object_->GetWorldTransform().worldPreMat_.GetWorldPosition();
	moveSmokeEmitter_->SetRengeMinMax(strSmoke, endSmoke);
	moveSmokeEmitter2_->SetRengeMinMax(strSmoke, endSmoke);



	if (count > 0 || phase_ > 0) {
		moveSmokeEmitter_->Update();
		moveSmokeEmitter2_->Update();

	}

	//if (enemy_->nullChek == Matrix4x4::Identity()) {
		enemyPos_ = enemy_->GetObject3D()->GetWorldPosition();
	//}

	if (isAlive_) {
		Vector3 norm;

		switch (phase_)
		{
		case 0:
			count += GetTimer();

			t = count;

			object_->GetWorldTransform().translate_ = Lerp(str, randPosSky, t);

			norm = randPosSky - str;

			velocity_ = norm * GetTimer();

			if (t >= 1) {
				t = 0;
				count = 0;
				phase_++;
			}

			break;
		case 1:



			count += GetTimer();
			if (count >= max_count)
			{
				Vector3 pos = enemyPos_ - object_->GetWorldPosition();

				float posLength = Length(pos);


				if (posLength > 500.0f) {
					moveSmokeEmitter_->SetLifeTimeMinMax(0.2f, 0.4f);
					moveSmokeEmitter_->SetCount(20);
					moveSmokeEmitter2_->SetCount(20);
				}
				else if (posLength > 100.0f) {
					moveSmokeEmitter_->SetLifeTimeMinMax(0.3f, 0.5f);
					moveSmokeEmitter_->SetCount(10);
					moveSmokeEmitter2_->SetCount(10);
				}
				else {
					moveSmokeEmitter_->SetLifeTimeMinMax(0.7f, 1.0f);
					moveSmokeEmitter_->SetCount(5);
					moveSmokeEmitter2_->SetCount(5);
				}

				Vector3 pos2 = pos;

				velocity_ = pos2;
				velocity_.y = 0;

				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();

				if (5 >= DistanceXZ(object_->GetWorldPosition(), enemyPos_)) {
					phase_++;
					posGround = enemyPos_;
					targetPos = posGround - object_->GetWorldPosition();
					count = 0;
				}

			}
			break;
		case 2:
			moveSmokeEmitter_->SetCount(5);
			moveSmokeEmitter2_->SetCount(5);

			count += GetTimer();
			if (count >= max_count)
			{
				velocity_ = targetPos * 3;

				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();
			}

			if (2.5f >= object_->GetWorldPosition().y) {
				isAlive_ = false;
				count = 0;
				phase_ = 0;
				hitEmitter_->SetIsEmit(true);
				hitEmitter_->Emit();
				isEffectPlay_ = true;

			}

			break;
		}
	}


	hitObject_->GetWorldTransform().translate_ = object_->GetWorldTransform().translate_;
	hitObject_->GetWorldTransform().rotate_.x = DegreesToRadians(-90);

	if (isEffectPlay_) {
		time_ += GetTimer();

		object_->SetIsDraw(false);
		hitObject_->SetIsDraw(true);
		hitObject_->GetWorldTransform().translate_ = object_->GetWorldTransform().translate_ + Vector3{ 0.0f,5.0f ,0.0f };
		hitObject_->GetWorldTransform().rotate_.x = DegreesToRadians(-90);
		hitObject_->GetWorldTransform().scale_ += Vector3(0.15f, 0.15f, 0.00f);
		hitObject_->GetPrimitive()->GetMaterial()->color.a -= 0.05f;

		if (time_ >= 0.5f) {
			isEffectPlay_ = false;
		}
	}

	hitEmitter_->Update();


	//colliderComponent_->UpdateAll(object_->worldtransform_);


	Vector3 velo = velocity_.Normalize();

	moveSmokeEmitter_->SetVelocityMinMax(-velo, -velo);


	// Y軸周り角度(θy)
	object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	// X軸周り角度(θx)
	object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);
}

void PlayerMissile::Draw()
{

}

void PlayerMissile::DrawP()
{
}

void PlayerMissile::Draw2D()
{
}

void PlayerMissile::InitMoveSmoke(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName)
{
	emitter->Initialize(particleManager, emitName, emitName, ParticleData::SpawnType::kSegmentLine);
	emitter->GetFrequency() = 0.00f;
	emitter->SetCount(2);
	emitter->SetLifeTimeMinMax(0.5f, 0.7f);
	emitter->SetIsAlpha(true);
	emitter->SetAlphaClipping(0.23f);
	emitter->SetIsLifeTimeScale(true);
	emitter->SetUsebillboard(true);
	emitter->SetUsebillboardRotZ(true);
	emitter->SetEnableLighting(false);
	emitter->SetIsLineInterpolation(true);
	emitter->SetColorMinMax({ 1,1,1,0.5f }, { 1,1,1,0.5f });
	emitter->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	emitter->SetSizeMinMax(Vector3{ 0.8f,0.8f,0.8f }, { 0.8f,0.8f,0.8f });
	emitter->SetVelocityMinMax(-velocity_, -velocity_);
	emitter->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
	emitter->SetRengeMinMax(object_->GetWorldTransform().translate_, object_->GetWorldTransform().translate_);
}
