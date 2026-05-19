#include "Effect.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"


void EffectSystem::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables)
{
	this->entityManager = entityManager;	// エンティティ3d
	this->globalVariables = globalVariables;	// 保存項目追加
	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<Engine::EffectComponent>();
	effectComponent_->Init(entityManager, globalVariables);
	// エフェクトエディタ初期化
	effectEditor_ = std::make_unique<EffectEditor>();
	effectEditor_->Initialize(effectComponent_.get(), globalVariables);


	// パーティクル初期化
	InitParticle();
	// 範囲弾パーティクル初期化 
	InitRangeBombingBullet();
	// 通常弾初期化
	InitBullet();
	
	for (auto& [name, data] : effectEditor_->GetEffectGlobalDatas()) {
		CreateEffect(name, data.shapeType, data);
	}
}


void EffectSystem::InitParticle() {
	// ヒットエミッター(形状:★)
	effectEditor_->AddEffectGlobalData("EmitterStarEmit", "hitStar");
	// ヒットエミッター(形状:Plane)
	effectEditor_->AddEffectGlobalData("EmitterHitEmit", "hit");
	// ヒットエミッター(形状:Plane)
	effectEditor_->AddEffectGlobalData("EmitterHitEffect", "hitEffect2");
	// ヒットエミッター(形状:Ring)
	effectEditor_->AddEffectGlobalData("EmitterRingHit", "ringHit");
	// タイトル用砂パーティクル
	effectEditor_->AddEffectGlobalData("EmitterDust", "dust3");
	// タイヤ砂埃パーティクル
	effectEditor_->AddEffectGlobalData("EmitterTireDust", "dust3");
	
	// タイヤエミッター
	effectEditor_->AddEffectGlobalData("EmitterScrapTire", "scrapTire");
	// ねじエミッター
	effectEditor_->AddEffectGlobalData("EmitterScrapScrew", "scrapScrew");
	// 鉄の棒エミッター
	effectEditor_->AddEffectGlobalData("EmitterScrapIronRod", "scrapIronRod");
	// 歯車エミッター
	effectEditor_->AddEffectGlobalData("EmitterScrapGear", "scrapGear");
	// 基盤エミッター
	effectEditor_->AddEffectGlobalData("EmitterScrapBasis", "scrapBasis");

	// 死んだとき

	// ねじエミッター
	effectEditor_->AddEffectGlobalData("EmitterDeathEnemyScrapScrew", "scrapScrew");
	// 鉄の棒エミッター
	effectEditor_->AddEffectGlobalData("EmitterDeathEnemyScrapIronRod", "scrapIronRod");
	// 歯車エミッター
	effectEditor_->AddEffectGlobalData("EmitterDeathEnemyScrapGear", "scrapGear");
	// 煙エミッター
	effectEditor_->AddEffectGlobalData("EmitterDeathEnemySmoke", "smokePlane05");
	// 爆破エミッター
	effectEditor_->AddEffectGlobalData("EmitterDeathEnemyExp", "expPlane01");

	// プレイヤー食らいエフェクト
	effectEditor_->AddEffectGlobalData("EmitterPlayerHitDamageEffect", "hit");

}

void EffectSystem::InitRangeBombingBullet() {
	// ミサイルヒットシリンダエミッター
	effectEditor_->AddEffectGlobalData("EmitterMissileHitCylinder", "missileHitCylinder");
	// 初期煙エミッター
	effectEditor_->AddEffectGlobalData("EmitterStratSmoke01", "smokePlane01");


	{
		effectComponent_->AddEmitter("missileHitCylinder", "missileHitCylinder", EmitterShapeType::POINT);
		Engine::PointParticleEmitter* missileHitCylinder = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("missileHitCylinder");
		missileHitCylinder->GetFrequency() = 0.0f;
		missileHitCylinder->SetCount(1, 0);
		missileHitCylinder->SetVelocity({ 0,0,0 }, { 0, 0, 0 });
		missileHitCylinder->SetPos({ 0,12.0f,0.0f });
		missileHitCylinder->SetLifeTime(0.5f, 0.0f);
		missileHitCylinder->SetRotate({ -Math::DegreesToRadians(90),Math::DegreesToRadians(0) ,0 }, {});
		missileHitCylinder->SetIsAlpha(true);
		missileHitCylinder->SetUsebillboard(false);
		missileHitCylinder->SetIsScaling(true);
		missileHitCylinder->SetSizeAmount({ 0.15f,0.15f ,0.0f }, {});
		missileHitCylinder->SetSize({ 1,1,1 }, {});
		missileHitCylinder->SetColorMinMax({ 1, 1, 1 }, { 1, 1, 1 });
	}

	{
		effectComponent_->AddEmitter("stratSmoke01", "smokePlane01", EmitterShapeType::POINT);
		Engine::PointParticleEmitter* stratSmoke01_ = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("stratSmoke01");
		stratSmoke01_->SetFrequency(0);
		stratSmoke01_->SetIsAlpha(true);
		stratSmoke01_->SetAlphaClipping(0.1f);
		stratSmoke01_->SetIsLifeTimeScale(true);
		stratSmoke01_->SetUsebillboard(true);
		stratSmoke01_->SetUsebillboardRotZ(true);
		stratSmoke01_->SetEnableLighting(false);
		stratSmoke01_->SetColorMinMax({ 1,1,1,0.5f }, { 1,1,1,0.5f });
		stratSmoke01_->SetSize(Vector3{ 55.8f,55.8f,55.8f }, {});
		stratSmoke01_->SetVelocity({ 0,25,0 }, { 160, 35, 160 });
		stratSmoke01_->SetRotate({}, Math::DegreesToRadians(Vector3{ 180,180,180 }));
	}
	
	// 初期煙エミッター
	effectEditor_->AddEffectGlobalData("EmitterSmokePlaneExpSmoke", "smokePlane05");


	effectComponent_->AddEmitter("smokePlaneExpSmoke", "smokePlane05", EmitterShapeType::POINT);
	Engine::PointParticleEmitter* smokePlaneExpSmoke = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("smokePlaneExpSmoke");

	smokePlaneExpSmoke->GetFrequency() = 0.00f;
	smokePlaneExpSmoke->SetCount(30, 0);
	smokePlaneExpSmoke->SetLifeTime(3.5f, 0);
	smokePlaneExpSmoke->SetIsAlpha(true);
	smokePlaneExpSmoke->SetAlphaClipping(0.23f);
	smokePlaneExpSmoke->SetIsLifeTimeScale(true);
	smokePlaneExpSmoke->SetUsebillboard(true);
	smokePlaneExpSmoke->SetUsebillboardRotZ(true);
	smokePlaneExpSmoke->SetEnableLighting(false);
	smokePlaneExpSmoke->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	smokePlaneExpSmoke->SetSize(Vector3{ 15.8f,15.8f,15.8f }, {});
	smokePlaneExpSmoke->SetVelocity(Vector3{ 00,50,00 }, { 40,50,40 });
	smokePlaneExpSmoke->SetRotate({}, Math::DegreesToRadians(Vector3{ 180,180,180 }));

	// 初期煙エミッター
	effectEditor_->AddEffectGlobalData("EmitterExpPlane01", "expPlane01");


	effectComponent_->AddEmitter("expPlane01", "expPlane01", EmitterShapeType::POINT);
	Engine::PointParticleEmitter* expPlane = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("expPlane01");


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
	expPlane->SetSize(Vector3{ 8.8f,8.8f,8.8f }, {});
	expPlane->SetVelocity({ 0,5,0 }, { 3, 10, 3 });
	expPlane->SetRotate({}, Math::DegreesToRadians(Vector3{ 180,180,180 }));

	// 初期煙エミッター
	effectEditor_->AddEffectGlobalData("EmitterAnimatedCube", "AnimatedCube");


	effectComponent_->AddEmitter("AnimatedCube", "AnimatedCube", EmitterShapeType::POINT);
	Engine::PointParticleEmitter* animatedCube = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("AnimatedCube");

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
	animatedCube->SetRotateVelocity({}, Math::DegreesToRadians(Vector3{ 180,180,180 }));
	animatedCube->SetSize(Vector3{ 0.5f,0.5f,0.5f }, {});
	animatedCube->SetVelocity(Vector3{ 0,30,0 }, { 40,20,40 });
	animatedCube->SetRotate({}, Math::DegreesToRadians(Vector3{ 180,180,180 }));


	// エミッター
	effectEditor_->AddEffectGlobalData("EmitterRingEmit", "ringEmit");


	effectComponent_->AddEmitter("ringEmit", "ringEmit", EmitterShapeType::AABB);
	Engine::AABBParticleEmitter* ringEmit = effectComponent_->GetEmitterAs<Engine::AABBParticleEmitter>("ringEmit");

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
	ringEmit->SetRotate(Math::DegreesToRadians(Vector3{ 90,0,0 }), Math::DegreesToRadians(Vector3{ 90,0,0 }));

	// エミッター
	effectEditor_->AddEffectGlobalData("EmitterExpSpark", "expSpark");


	effectComponent_->AddEmitter("expSpark", "expSpark", EmitterShapeType::POINT);
	Engine::PointParticleEmitter* expSpark = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("expSpark");

	expSpark->GetFrequency() = 0.00f;
	expSpark->SetCount(1, 0);
	expSpark->SetLifeTime(0.15f, 0.00f);
	expSpark->SetIsAlpha(true);
	expSpark->SetAlphaClipping(0.10f);
	expSpark->SetUsebillboard(true);
	expSpark->SetColorMinMax({ 1.0f,1.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	expSpark->SetSize(Vector3{ 15.8f,15.8f,15.8f }, {});
	expSpark->SetVelocity(Vector3{ 0,0,0 }, { 0,0,0 });
	expSpark->SetRotate(Math::DegreesToRadians(Vector3{ 90,0,0 }), Math::DegreesToRadians(Vector3{ 90,0,0 }));


	// 初期煙エミッター
	effectEditor_->AddEffectGlobalData("EmitterMissileHit", "missileHit");

	effectComponent_->AddEmitter("missileHit", "missileHit", EmitterShapeType::POINT);
	Engine::PointParticleEmitter* hitEmitter_ = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("missileHit");

	Vector3 size = { 2.0f, 2.0f, 2.0f };
	hitEmitter_->SetCount(2, 0);
	hitEmitter_->SetLifeTime(0.25f, 0);
	hitEmitter_->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	hitEmitter_->SetSize(size, size);
	hitEmitter_->SetFrequency(0.00f);
	hitEmitter_->SetPos({ 0,0,0 });
	hitEmitter_->SetUsebillboardRotZ(true);
	hitEmitter_->SetIsAlpha(true);
	hitEmitter_->SetVelocity({}, {});
	hitEmitter_->SetRotate({ 0,0,0 }, { 0,0,3.14f });
	hitEmitter_->SetIsEmit(false);
}

void EffectSystem::InitBullet()
{
	effectComponent_->AddEmitter("bulletSmoke", "smokePlane03_2", EmitterShapeType::POINT);
	Engine::PointParticleEmitter* starEmit = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("bulletSmoke");
	starEmit->GetFrequency() = 0.0f;
	starEmit->SetCount(10, 0);
	starEmit->SetPos({ 0,0.0f,0.0f });
	starEmit->SetRotate({}, Math::DegreesToRadians({ 180,180,180 }));
	starEmit->SetAlphaClipping(0.10f);
	Vector3 velo = { 2,2,2 };

	starEmit->SetVelocity({}, velo);
	starEmit->SetLifeTime(1.25f, 0);
	starEmit->SetIsAlpha(true);
	starEmit->SetSize(Vector3{ 0.8f,0.8f,0.8f }, { 0.1f,0.1f,0.1f });
	starEmit->SetColorMinMax({ 0.424f, 0.404f, 0.431f }, { 0.424f, 0.404f, 0.431f });



	effectComponent_->AddEmitter("cartridge", "cartridge", EmitterShapeType::POINT);
	Engine::PointParticleEmitter* cartridgeEmit = effectComponent_->GetEmitterAs<Engine::PointParticleEmitter>("cartridge");
	cartridgeEmit->GetFrequency() = 0.0f;
	cartridgeEmit->SetCount(1, 0);
	cartridgeEmit->SetPos({ 0,0.0f,0.0f });
	cartridgeEmit->SetRotate({}, Math::DegreesToRadians({ 180,180,180 }));
	cartridgeEmit->SetAlphaClipping(0.10f);
	cartridgeEmit->SetUsebillboard(false);
	velo = { 1,1,1 };

	cartridgeEmit->SetVelocity({ 0,5,0 }, velo);
	cartridgeEmit->SetLifeTime(2.0f, 0);
	cartridgeEmit->SetIsAlpha(true);
	cartridgeEmit->SetIsGravity(true);
	cartridgeEmit->SetIsBounce(true);
	cartridgeEmit->SetSize(Vector3{ 0.8f,0.8f,0.8f }, {});
	cartridgeEmit->SetColorMinMax({ 1,1, 0 }, { 1,1, 0 });
}


void EffectSystem::Update(float dt) {
	// エフェクトエディタ更新
	effectEditor_->Update(dt);
	// 更新
	effectComponent_->Update();
}

#pragma region Emit

void EffectSystem::Emit(const std::string& name, const Vector3& pos)
{
	Engine::BaseParticleEmitter* emit = effectComponent_->GetBaseEmitter(name);
	if (emit == nullptr) return;	// エミッターが存在しない場合は終了
	// 出現
	emit->SetPos(pos);		// 位置
	emit->SetIsEmit(false); // 出さない
	emit->Update();			// 更新
	emit->SetIsEmit(true);	// 出す
	emit->Emit();			// エフェクト出現
	emit->SetIsEmit(false); // 出さない
}

void EffectSystem::Emit(const std::string& name, const Vector3& pos, const Vector3& dir, const Vector3& range) {
	Engine::BaseParticleEmitter* emit = effectComponent_->GetBaseEmitter(name);
	if (emit == nullptr) return;	// エミッターが存在しない場合は終了
	// 出現
	emit->SetPos(pos);		// 位置
	emit->SetVelocity(dir, range);	// 速度
	emit->SetIsEmit(false); // 出さない
	emit->Update();			// 更新
	emit->SetIsEmit(true);	// 出す
	emit->Emit();			// エフェクト出現
	emit->SetIsEmit(false); // 出さない
}

#pragma endregion // 出現

void EffectSystem::CreateEffect(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data) {
	effectComponent_->AddEmitter(name, data.particleName, shapeType);
	effectEditor_->SetEffectGlobalData(name, shapeType, data);
}
