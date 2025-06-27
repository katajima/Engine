#include "PlayerEffect.h"

#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"


void PlayerEffect::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	dashEmitter_ = std::make_unique <ParticleEmitter>();
	dashEmitter_->Initialize(particleManager, "dash", "dashEmit", ParticleData::SpawnType::kCornerLine);
	dashEmitter_->GetFrequency() = 0.05f;
	dashEmitter_->SetCount(5);
	dashEmitter_->SetLifeTimeMinMax(0.1f, 0.1f);
	dashEmitter_->SetIsAlpha(true);
	dashEmitter_->SetIsEmit(false);
	dashEmitter_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	dashEmitter_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	dashEmitter_->SetSizeMinMax(Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	dashEmitter_->SetVelocityMinMax({}, {});
	dashEmitter_->SetPos({ 0,7,0 });
	dashEmitter_->SetCorner(16, 0.5f);
	dashEmitter_->transform_.rotate_.x = DegreesToRadians(90);



	trailEffect_ = std::make_unique<TrailEffect>();
	trailEffect_->Initialize(entity3DManager_->GetEffectManager(), "resources/Texture/uvChecker.png", 0.2f, Color{ 1,0,0,0.5f });
	trailEffect_->SetCamera(camera);
	
	isTrail_ = false;



	weaponStr.Initialize(entity3DManager);
	

	weaponEnd.Initialize(entity3DManager);
	
}

void PlayerEffect::Update()
{
	//trailEffect_->Update(isTrail_, weaponStr, weaponEnd);

	dashEmitter_->Update();
	weaponStr.Update();
	weaponEnd.Update();
}

void PlayerEffect::Draw()
{
	//trailEffect_->Draw();
}

void PlayerEffect::SetTrailParent(Object3d* object)
{
	weaponStr.worldtransform_.parent_ = &object->worldtransform_;
	weaponStr.worldtransform_.translate_ = { 0,object->GetMesh(0)->GetMax().y ,0 };

	weaponEnd.worldtransform_.parent_ = &object->worldtransform_;
	weaponEnd.worldtransform_.translate_ = { 0,object->GetMesh(0)->GetMin().y ,0 };
	weaponEnd.worldtransform_.translate_ = { 0,2 ,0 };
}
