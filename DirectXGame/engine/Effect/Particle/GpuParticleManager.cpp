#include "GpuParticleManager.h"

#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"

void GpuParticleManager::Initialize(Entity3DManager* entity3DManager, EffectManager* effectManager, SrvManager* srvManager)
{
	entity3DManager_ = entity3DManager;
	effectManager_ = effectManager;
	srvManager_ = srvManager;



	particleResource_ = effectManager_->GetDxCommon()->GetDXGIDevice()->CreateBufferResource(sizeof(ParticleCS) * 1024);
	//particleResource_->Map(0, nullptr, reinterpret_cast<void**>(&particleCS_));
	particleSrvIndex_ = srvManager_->Allocate();


	preViewResource_ = effectManager_->GetDxCommon()->GetDXGIDevice()->CreateBufferResource(sizeof(PreView));
	preViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&preView_));
}
