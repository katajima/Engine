#include "Effect.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void Effect::Initialize(Entity3DManager* entity3DManager, GlobalVariables* globalVariables)
{
	entity3DManager_ = entity3DManager;
	globalVariables_ = globalVariables;


	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<EffectComponent>();
	effectComponent_->Init(entity3DManager_, globalVariables_);

	InitParticle();
	InitRangeBombingBullet();
}

void Effect::InitParticle(){

	effectComponent_->AddEmitter("starEmit","hitStar",EmitterShapeType::POINT);
	PointParticleEmitter* starEmit = effectComponent_->GetEmitterAs<PointParticleEmitter>("starEmit");
	starEmit->GetFrequency() = 0.0f;
	starEmit->SetCount(1, 0);
	//starEmit->SetParent(GetObject3D()->GetWorldTransform());
	starEmit->SetPos({ 0,0.0f,0.0f });
	starEmit->SetRotate({}, DegreesToRadians({ 180,180,180 }));
	starEmit->SetVelocity({}, {});
	starEmit->SetLifeTime(0.2f, 0);
	starEmit->SetIsAlpha(true);
	starEmit->SetUsebillboard(false);
	starEmit->SetSize(Vector3{ 1.7f,1.7f,1.7f }, { 0.1f,0.1f,0.1f });
	starEmit->SetColorMinMax({ 0.424f, 0.404f, 0.431f }, { 0.424f, 0.404f, 0.431f });
	//starEmit->SetIsEmit(true);


	effectComponent_->AddEmitter("traiEmit", "hitEffect", EmitterShapeType::AABB);
	AABBParticleEmitter* traiEmit = effectComponent_->GetEmitterAs<AABBParticleEmitter>("traiEmit");
	traiEmit->GetFrequency() = 0.0f;
	traiEmit->SetCount(5, 0);
	//traiEmit->SetParent(worldEffect_);
	traiEmit->SetPos({ 0,0.0f,0.0f });
	traiEmit->SetRotate({}, DegreesToRadians({ 180,180,180 }));
	traiEmit->SetVelocity({ 0,0,0 }, { 0, 0, 0 });
	traiEmit->SetLifeTime(0.2f, 0);
	traiEmit->SetIsAlpha(true);
	traiEmit->SetUsebillboard(false);
	traiEmit->SetSize(Vector3{ 2.6f,2.6f,2.6f }, { 0.1f,0.1f,0.1f });
	traiEmit->SetColorMinMax({ 1, 0, 0 }, { 1, 1, 0 });
	traiEmit->SetRange(Vector3{ -5,-5,-5 }, Vector3{ 5,5,5 });
	traiEmit->SetAlphaClipping(0.15f);
	//traiEmit->SetIsEmit(true);


	effectComponent_->AddEmitter("hitEmit", "hit", EmitterShapeType::POINT);
	PointParticleEmitter* hitEmit = effectComponent_->GetEmitterAs<PointParticleEmitter>("hitEmit");

	hitEmit->GetFrequency() = 0.0f;
	hitEmit->SetCount(10, 0);
	//hitEmit->SetParent(worldEffect_);
	hitEmit->SetPos({ 0,0.0f,0.0f });
	hitEmit->SetVelocity({}, { 15,15,15 });
	hitEmit->SetLifeTime(0.5f, 0.1f);
	hitEmit->SetIsAlpha(true);
	hitEmit->SetUsebillboard(true);
	hitEmit->SetUsebillboardRotZ(true);
	hitEmit->SetSize(Vector3{ 2.0f,1.0f,1.0f }, { 0.1f,0.1f,0.0f });
	hitEmit->SetColorMinMax({ 1, 1,  1 }, { 1, 1, 1 });
	//hitEmit->SetIsEmit(true);
	hitEmit->SetIsDirectionRotate(true);


	effectComponent_->AddEmitter("hitEffect2", "hitEffect2", EmitterShapeType::POINT);
	PointParticleEmitter* effectEmit = effectComponent_->GetEmitterAs<PointParticleEmitter>("hitEffect2");

	effectEmit->GetFrequency() = 0.0f;
	effectEmit->SetCount(1, 0);
	//effectEmit->SetParent(worldEffect_);
	effectEmit->SetPos({ 0,0.0f,0.0f });
	effectEmit->SetRotate({}, DegreesToRadians({ 180,180,180 }));
	effectEmit->SetVelocity({ 0,0,0 }, { 0, 0, 0 });
	effectEmit->SetLifeTime(0.2f, 0.1f);
	effectEmit->SetIsAlpha(true);
	effectEmit->SetUsebillboard(true);
	effectEmit->SetSize({ 8,8,8 }, {});
	effectEmit->SetColorMinMax({ 1, 0, 0 }, { 1, 1, 0 });
	effectEmit->SetAlphaClipping(0.15f);
	//effectEmit->SetIsEmit(true);

	effectComponent_->AddEmitter("ringHit", "ringHit", EmitterShapeType::POINT);
	PointParticleEmitter* hitRingEmit = effectComponent_->GetEmitterAs<PointParticleEmitter>("ringHit");

	hitRingEmit->GetFrequency() = 0.0f;
	hitRingEmit->SetCount(1, 0);
	//hitRingEmit->SetParent(worldEffect_);
	hitRingEmit->SetPos({ 0,0.0f,0.0f });
	hitRingEmit->SetRotate({}, DegreesToRadians({ 180,180,180 }));
	hitRingEmit->SetVelocity({ 0,0,0 }, { 0, 0, 0 });
	hitRingEmit->SetLifeTime(0.4f, 0.1f);
	hitRingEmit->SetIsAlpha(true);
	hitRingEmit->SetUsebillboard(true);
	hitRingEmit->SetIsLifeTimeScale(true);
	hitRingEmit->SetLifeTimeScaleTopBottom(EmitData::TopBottom::kTop);
	hitRingEmit->SetSize({ 8,8,8 }, {});
	hitRingEmit->SetColorMinMax({ 1, 1, 1 }, { 1, 1, 1 });
	hitRingEmit->SetAlphaClipping(0.15f);
	//hitRingEmit->SetIsEmit(true);



	effectComponent_->AddEmitter("missileHitCylinder", "missileHitCylinder", EmitterShapeType::POINT);
	PointParticleEmitter* missileHitCylinder = effectComponent_->GetEmitterAs<PointParticleEmitter>("missileHitCylinder");
	missileHitCylinder->GetFrequency() = 0.0f;
	missileHitCylinder->SetCount(1, 0);
	missileHitCylinder->SetVelocity({ 0,0,0 }, { 0, 0, 0 });
	missileHitCylinder->SetPos({ 0,0.0f,0.0f });
	missileHitCylinder->SetPos({ 0,0.0f,0.0f });
	missileHitCylinder->SetLifeTime(0.5f, 0.0f);
	missileHitCylinder->SetRotate({ -DegreesToRadians(90),DegreesToRadians(0) ,0}, {});
	missileHitCylinder->SetIsAlpha(true);
	missileHitCylinder->SetUsebillboard(false);


	missileHitCylinder->SetIsScaling(true);
	missileHitCylinder->SetSizeAmount({0.15f,0.15f ,0.0f},{});
	missileHitCylinder->SetSize({ 1,1,1 }, {});
	missileHitCylinder->SetColorMinMax({ 1, 1, 1 }, { 1, 1, 1 });
}

void Effect::InitRangeBombingBullet()
{
	effectComponent_->AddEmitter("stratSmoke01", "smokePlane01", EmitterShapeType::POINT);
	effectComponent_->AddEmitter("stratSmoke02", "smokePlane05", EmitterShapeType::POINT);
	PointParticleEmitter* stratSmoke01_ = effectComponent_->GetEmitterAs<PointParticleEmitter>("stratSmoke01");
	PointParticleEmitter* stratSmoke02_ = effectComponent_->GetEmitterAs<PointParticleEmitter>("stratSmoke02");


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

	effectComponent_->AddEmitter("missileHit", "missileHit", EmitterShapeType::POINT);
	PointParticleEmitter* hitEmitter_ = effectComponent_->GetEmitterAs<PointParticleEmitter>("missileHit");

	Vector3 size = { 2.0f, 2.0f, 2.0f };
	hitEmitter_->SetCount(2, 0);
	hitEmitter_->SetLifeTime(0.25f, 0);
	hitEmitter_->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	hitEmitter_->SetSize(size, size);
	hitEmitter_->SetFrequency(0.00f);
	//hitEmitter_->SetParent(object_->GetWorldTransform());
	hitEmitter_->SetPos({ 0,0,0 });
	hitEmitter_->SetUsebillboardRotZ(true);
	hitEmitter_->SetIsAlpha(true);
	hitEmitter_->SetVelocity({}, {});
	hitEmitter_->SetRotate({ 0,0,0 }, { 0,0,3.14f });
	hitEmitter_->SetIsEmit(false);
}

void Effect::Update() {
	effectComponent_->Update();
}

void Effect::Emit(const std::string& name, const Vector3& pos)
{
	BaseParticleEmitter* emit = effectComponent_->GetBaseEmitter(name);
	
	emit->SetPos(pos);
	emit->SetIsEmit(false);
	emit->Update();
	emit->SetIsEmit(true);
	emit->Emit();
	emit->SetIsEmit(false);
}

