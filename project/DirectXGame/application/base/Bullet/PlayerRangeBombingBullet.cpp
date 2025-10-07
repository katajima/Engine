#include "PlayerRangeBombingBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"

PlayerRangeBombingBullet::~PlayerRangeBombingBullet()
{
	effectComponent_->GetGpuParticleManager()->ClearEmitter("MoveEmitte_" + std::to_string(countIndex_));
	effectComponent_->RemoveEmitterAll();
	effectComponent_->RemovePrimitiveAll();
	effectComponent_->RemoveTrailEffectAll();
}

void PlayerRangeBombingBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	globalVariables_ = globalVariables;
	entity3DManager_ = entity3DManager;
	//// オブジェクト設定
	object_ = entity3DManager->CreateObject3D("playerbullet", ObjectModelType::kNormal, position, camera);
	object_->SetModel("player_bullet.obj");
	//object_->UseTrailEffect("resources/Texture/Image.png", 0.15f, { 1.0f,1.0f,1.0f,1.0f }, { 0,1.5f,0 }, { 0,-1.5f,0 });
	object_->Update();
	object_->InitColliderComponent();

	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;
	sphere->layer = CollisionLayer::PlayerAttack;
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	sphere->radius = 10.0f; // 半径を適宜設定
	sphere->Disable(); // 初期状態では無効化
	object_->GetColliderComponent()->AddCollider(std::move(sphere));



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
		enemy->Emit();
		};


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
	randPosSky = str + Vector3{ -1000,0,0 };

	max_y = 600.0f; // 最大Y座標

	randPosSky.y = max_y;



	//posGround = targetRange_.position;

	effectComponent_ = std::make_unique<EffectComponent>();
	effectComponent_->Init(entity3DManager_, globalVariables_);
	effectComponent_->AddTrailEffect("trail", "resources/Texture/Image.png", 0.5f, object_->GetWorldTransform(), { 1.0f,1.0f,1.0f,1.0f }, { 0,1.5f,0 }, { 0,-1.5f,0 });

	//// 各演出設定
	//// ミサイル移動中の煙パーティクルエミッター

	countIndex_ = UniqueIdGenerator::Generate();

	// 移動中の煙エフェクト
	InitMoveSmoke();
	// 開始時の煙エフェクト
	InitStartSmoke();
	// 爆発時の煙エフェクト
	InitExpSmoke();
	// 爆発時の火エフェクト
	InitExpFire();
	// 爆発時の破片エフェクト
	InitExpBre();
	// リングエフェクト
	InitRingEmitter();
	// ヒットエフェクト
	InitHitEmitter();
	


	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = cilnderHeight_;
	cylinderParam.innerRadius = 6.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;


	std::unique_ptr<CylinderPrimitive> cylinder2 = std::make_unique<CylinderPrimitive>();
	cylinder2->Data() = cylinderParam;
	cylinder2->Initialize(entity3DManager_->GetPrimitiveCommon(), "resources/Texture/effect/gradationLine.png");


	cylinderParam.height = 10.0f;
	cylinderParam.innerRadius = 12.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = true;

	hitObject2_ = entity3DManager->CreatePrimitiveObject3D<CylinderPrimitive>("cylinder", "resources/Texture/Image.png", camera);
	hitObject2_->SetPrimitive(std::move(cylinder2));
	hitObject2_->GetPrimitive()->SetPsoType(BasePrimitive::PsoType::kRingClamp);
	hitObject2_->SetIsDraw(false);
	hitObject2_->GetPrimitive()->GetMaterial()->color = { 1.0f,0.0f,0.0f,0.5f };

	enemyPos_.x = targetRange_.position.x + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
	enemyPos_.z = targetRange_.position.z + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
	enemyPos_.y = 2.0f;
}

void PlayerRangeBombingBullet::Update()
{
	effectComponent_->GetTrailEffect("trail")->SetIsEmit(true);
	emitterPoint->GetCommonData()->emit = false;

	// 時間経過でデス
	if (Hit) {
		effect_->Emit("missileHit", object_->GetWorldTransform().worldMat_.GetWorldPosition());
		isAlive_ = false;
	}

	Vector3 strSmoke = object_->GetWorldTransform().worldMat_.GetWorldPosition();
	Vector3 endSmoke = object_->GetWorldTransform().worldPreMat_.GetWorldPosition();

	emitterPoint->GetWorldTransform().translate_ = object_->GetWorldPosition();
	emitterPoint->GetCommonData()->prevTranslate = object_->GetPreWorldPosition();


	if (count > 0 || phase_ > 0) {

	}

	if (isAlive_) {
		Vector3 norm;
		switch (phase_)
		{
		case 0:
			if (t > 0.0f) {
				flag_ = true;
			}
			else {
				effect_->Emit("stratSmoke01", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 10.0f,-5.0f,0.0f });
			}

			count += GetTimer() * 2.0f;

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

			flag_ = true;

			count += GetTimer();
			if (count >= max_count)
			{
				Vector3 pos = enemyPos_ - object_->GetWorldPosition();

				float posLength = Length(pos);

				emitterPoint->GetCommonData()->emit = true;

				
				if (posLength > 500.0f) {
					emitterPoint->GetCommonData()->count = 5;

				}
				else if (posLength > 100.0f) {
					emitterPoint->GetCommonData()->count = 5;
				}
				else {
					emitterPoint->GetCommonData()->count = 5;

				}

				Vector3 pos2 = pos;

				velocity_ = pos2.Normalize() * 1000;
				velocity_.y = 0;

				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();

				if (5 >= DistanceXZ(object_->GetWorldPosition(), enemyPos_)) {
					phase_++;
					posGround = enemyPos_;


					targetPos = posGround - object_->GetWorldPosition();
					count = 0;

					hitObject2_->SetIsDraw(true);
					emitterPoint->GetCommonData()->emit = false;
					emitterPoint->GetCommonData()->lifeTime = 0.01f;
				}
				
			}
			break;
		case 2:
			flag_ = true;
			
			
			// リングエフェクトの位置を設定
			effect_->Emit("ringEmit", object_->GetWorldTransform().worldMat_.GetWorldPosition());

	
			count += GetTimer();
			if (count >= max_count)
			{
				velocity_ = targetPos * 3;
				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();
			}

			if (-0.0f >= object_->GetWorldPosition().y) {

				object_->GetWorldTransform().translate_.y = 0.5f;
				isAlive_ = false;
				count = 0;
				phase_ = 0;
				effect_->Emit("missileHit", object_->GetWorldTransform().worldMat_.GetWorldPosition());

				isEffectPlay_ = true;

				object_->GetWorldTransform().Update();
				effect_->Emit("missileHitCylinder", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 0,10,0 });

				flag_ = false;
				hitObject2_->SetIsDraw(false);

				effect_->Emit("smokePlaneExpSmoke", object_->GetWorldTransform().worldMat_.GetWorldPosition());

				effect_->Emit("AnimatedCube", object_->GetWorldTransform().worldMat_.GetWorldPosition());

				effect_->Emit("expPlane01", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 0,5,0 });
			
			
				effect_->Emit("expSpark", object_->GetWorldTransform().worldMat_.GetWorldPosition() );
			
			
			}

			break;
		}

		Vector3 velo = velocity_.Normalize();

		emitterPoint->GetCommonData()->velocity = -velo * 10.0f;


		// Y軸周り角度(θy)
		object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
		float length = Length(Vector3(velocity_.x, 0, velocity_.z));
		// X軸周り角度(θx)
		object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);


		hitObject2_->GetWorldTransform().translate_ = posGround + Vector3{ 0,-6.0f,0 };




		hitObject2_->GetWorldTransform().rotate_.x = DegreesToRadians(-90);
	}




	if (isEffectPlay_) {
		time_ += GetTimer();
		object_->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
		object_->SetIsDraw(false);



		if (time_ >= 0.5f) {
			object_->IsDelete();
			hitObject2_->IsDelete();
			isEffectPlay_ = false;
			object_->GetColliderComponent()->ClearColliders();


		}



	}

	effectComponent_->Update();


}

void PlayerRangeBombingBullet::Draw()
{

}

void PlayerRangeBombingBullet::DrawP()
{
	effectComponent_->Draw();
	effectComponent_->DrawEffect();
}

void PlayerRangeBombingBullet::Draw2D()
{
}
//
void PlayerRangeBombingBullet::InitStartSmoke()
{
	effectComponent_->AddEmitter("smokePlane01", "smokePlane01", EmitterShapeType::POINT);
	effectComponent_->AddEmitter("smokePlane05", "smokePlane05", EmitterShapeType::POINT);
	PointParticleEmitter* stratSmoke01_ = effectComponent_->GetEmitterAs<PointParticleEmitter>("smokePlane01");
	PointParticleEmitter* stratSmoke02_ = effectComponent_->GetEmitterAs<PointParticleEmitter>("smokePlane05");


	stratSmoke01_->SetFrequency(0);
	stratSmoke01_->SetIsAlpha(true);
	stratSmoke01_->SetAlphaClipping(0.1f);
	stratSmoke01_->SetIsLifeTimeScale(true);
	stratSmoke01_->SetUsebillboard(true);
	stratSmoke01_->SetUsebillboardRotZ(true);
	stratSmoke01_->SetEnableLighting(false);
	stratSmoke01_->SetColorMinMax({ 1,1,1,0.5f }, { 1,1,1,0.5f });
	//stratSmoke01_->Set({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	stratSmoke01_->SetSize(Vector3{ 55.8f,55.8f,55.8f }, {});
	stratSmoke01_->SetVelocity({ 0,25,0 }, { 160, 35, 160 });
	stratSmoke01_->SetRotate({}, DegreesToRadians(Vector3{ 180,180,180 }));

	stratSmoke02_->SetIsAlpha(true);
	stratSmoke02_->SetAlphaClipping(0.1f);
	stratSmoke02_->SetIsLifeTimeScale(true);
	stratSmoke02_->SetUsebillboard(true);
	stratSmoke02_->SetUsebillboardRotZ(true);
	stratSmoke02_->SetEnableLighting(false);
	stratSmoke02_->SetColorMinMax({ 1,1,1,0.5f }, { 1,1,1,0.5f });
	//stratSmok201_->Set({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	stratSmoke02_->SetSize(Vector3{ 55.8f,55.8f,55.8f }, {});
	stratSmoke02_->SetVelocity({ 0,25,0 }, { 160, 35, 160 });
	stratSmoke02_->SetRotate({}, DegreesToRadians(Vector3{ 180,180,180 }));
}

void PlayerRangeBombingBullet::InitMoveSmoke()
{
	effectComponent_->AddEmitter("smokePlane04", "smokePlane04", EmitterShapeType::LINE);
	//LineParticleEmitter* moveSmoke = effectComponent_->GetEmitterAs<LineParticleEmitter>("smokePlane04");

	effectComponent_->AddGPUParticleEmitter("MoveEmitte_" + std::to_string(countIndex_), "no3", EmitterType::Point);
	emitterPoint = effectComponent_->GetGpuParticleManager()->GetGpuParticleEmitter<GpuParticleEmitterPoint>("MoveEmitte_" + std::to_string(countIndex_));

	emitterPoint->GetCommonData()->color = { 1,1,1 };
	emitterPoint->GetCommonData()->frequency = 0.0f;
	emitterPoint->GetCommonData()->count = 50;
	emitterPoint->GetCommonData()->useBillboard = true;
	emitterPoint->GetCommonData()->scale = { 5.8f ,5.8f ,5.8f };
	emitterPoint->GetCommonData()->isAlhpa = true;
	emitterPoint->GetCommonData()->rotateRange = DegreesToRadians(Vector3{ 180,180,180 });
	emitterPoint->GetCommonData()->lifeTime = 0.6f;
	emitterPoint->GetCommonData()->velocity = -velocity_;
	emitterPoint->GetCommonData()->isScaling = true;
	emitterPoint->GetCommonData()->scaleAmount = 1.25f;
	emitterPoint->GetData()->interpolation = EmitterInterpolation::Sequential;
	emitterPoint->GetCommonData()->color = { 0.35f,0.35f,0.35f };
}

void PlayerRangeBombingBullet::InitExpSmoke()
{

	effectComponent_->AddEmitter("smokePlaneExpSmoke", "smokePlane05", EmitterShapeType::POINT);
	PointParticleEmitter* smokePlaneExpSmoke = effectComponent_->GetEmitterAs<PointParticleEmitter>("smokePlaneExpSmoke");

	smokePlaneExpSmoke->GetFrequency() = 0.00f;
	smokePlaneExpSmoke->SetCount(10, 0);
	smokePlaneExpSmoke->SetLifeTime(2.5f, 0);
	smokePlaneExpSmoke->SetIsAlpha(true);
	smokePlaneExpSmoke->SetAlphaClipping(0.23f);
	smokePlaneExpSmoke->SetIsLifeTimeScale(true);
	smokePlaneExpSmoke->SetUsebillboard(true);
	smokePlaneExpSmoke->SetUsebillboardRotZ(true);
	smokePlaneExpSmoke->SetEnableLighting(false);
	smokePlaneExpSmoke->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	//smokePlaneExpSmoke->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	smokePlaneExpSmoke->SetSize(Vector3{ 15.8f,15.8f,15.8f }, {});
	smokePlaneExpSmoke->SetVelocity(Vector3{ 30,50,30 }, { 0,50,0 });
	smokePlaneExpSmoke->SetRotate({}, DegreesToRadians(Vector3{ 180,180,180 }));

}

void PlayerRangeBombingBullet::InitExpFire()
{
	effectComponent_->AddEmitter("expPlane01", "expPlane01", EmitterShapeType::POINT);
	PointParticleEmitter* expPlane = effectComponent_->GetEmitterAs<PointParticleEmitter>("expPlane01");


	expPlane->GetFrequency() = 0.00f;
	expPlane->SetCount(2, 0);
	expPlane->SetLifeTime(0.26f, 0.01f);
	expPlane->SetIsAlpha(true);
	expPlane->SetAlphaClipping(0.23f);
	expPlane->SetIsLifeTimeScale(true);
	expPlane->SetUsebillboard(true);
	expPlane->SetUsebillboardRotZ(true);
	expPlane->SetEnableLighting(false);
	expPlane->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	//expPlane->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	expPlane->SetSize(Vector3{ 8.8f,8.8f,8.8f }, {});
	expPlane->SetVelocity({ 0,5,0 }, { 3, 10, 3 });
	expPlane->SetRotate({}, DegreesToRadians(Vector3{ 180,180,180 }));
}

void PlayerRangeBombingBullet::InitExpBre()
{
	effectComponent_->AddEmitter("AnimatedCube", "AnimatedCube", EmitterShapeType::POINT);
	PointParticleEmitter* animatedCube = effectComponent_->GetEmitterAs<PointParticleEmitter>("AnimatedCube");

	animatedCube->GetFrequency() = 0.00f;
	animatedCube->SetCount(6, 0);
	animatedCube->SetLifeTime(3.26f, 0.01f);
	animatedCube->SetAlphaClipping(0.23f);
	animatedCube->SetIsLifeTimeScale(true);
	animatedCube->SetLifeTimeScaleTopBottom(EmitData::TopBottom::kBottom);
	animatedCube->SetIsGravity(true);
	animatedCube->SetUsebillboard(false);
	animatedCube->SetUsebillboardRotZ(true);
	animatedCube->SetEnableLighting(false);
	animatedCube->SetColorMinMax({ 1.0f,0.5f,0.0f,1.0f }, { 1.0f,1.0f,0.0f,1.0f });
	animatedCube->SetIsAcceleration(true);
	animatedCube->SetIsBounce(true);
	animatedCube->SetIsRotateVelocity(true);
	animatedCube->SetRotateVelocity({}, DegreesToRadians(Vector3{ 180,180,180 }));
	animatedCube->SetSize(Vector3{ 0.5f,0.5f,0.5f }, {});
	animatedCube->SetVelocity(Vector3{ 0,30,0 }, { 40,20,40 });
	animatedCube->SetRotate({}, DegreesToRadians(Vector3{ 180,180,180 }));
}

void PlayerRangeBombingBullet::InitRingEmitter()
{
	effectComponent_->AddEmitter("ringEmit", "ringEmit", EmitterShapeType::AABB);
	AABBParticleEmitter* ringEmit = effectComponent_->GetEmitterAs<AABBParticleEmitter>("ringEmit");

	ringEmit->GetFrequency() = 0.00f;
	ringEmit->SetCount(3, 0);
	ringEmit->SetLifeTime(0.15f, 0.05f);
	ringEmit->SetIsAlpha(true);
	ringEmit->SetAlphaClipping(0.23f);
	ringEmit->SetIsLifeTimeScale(true);
	ringEmit->SetUsebillboard(false);
	ringEmit->SetUsebillboardRotZ(false);
	ringEmit->SetEnableLighting(false);
	ringEmit->SetRange({ 0.0f,-4.0f ,0.0f }, { 0.0f,20.0f,0.0f });
	ringEmit->SetColorMinMax({ 1.0f,1.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	ringEmit->SetSize(Vector3{ 5.8f,5.8f,5.8f }, {});
	ringEmit->SetVelocity(Vector3{ 0,0,0 }, { 0,0,0 });
	ringEmit->SetRotate(DegreesToRadians(Vector3{ 90,0,0 }), DegreesToRadians(Vector3{ 90,0,0 }));
}

// ヒット
void PlayerRangeBombingBullet::InitHitEmitter()
{
	effectComponent_->AddEmitter("missileHit", "missileHit", EmitterShapeType::POINT);
	PointParticleEmitter* hitEmitter_ = effectComponent_->GetEmitterAs<PointParticleEmitter>("missileHit");

	Vector3 size = { 2.0f, 2.0f, 2.0f };
	hitEmitter_->SetCount(2, 0);
	hitEmitter_->SetLifeTime(0.25f, 0);
	hitEmitter_->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	hitEmitter_->SetSize(size, size);
	hitEmitter_->SetFrequency(0.00f);
	hitEmitter_->SetParent(object_->GetWorldTransform());
	hitEmitter_->SetPos({ 0,0,0 });
	hitEmitter_->SetUsebillboardRotZ(true);
	hitEmitter_->SetIsAlpha(true);
	hitEmitter_->SetVelocity({}, {});
	hitEmitter_->SetRotate({ 0,0,0 }, { 0,0,3.14f });
	hitEmitter_->SetIsEmit(false);
}

