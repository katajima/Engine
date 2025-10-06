#pragma once

#include "EmitData.h"
#include "DirectXGame/engine/Effect/Particle/CPU/ParticleData.h"
#include<random>

namespace EmitFanction {
	void CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos);
	void CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos, Vector3 direction);
	void CreateParticle(ParticleGroup2d& group, EffectEmitData2d emitData, std::mt19937& randomEngine, Vector3 emitPos);

}


