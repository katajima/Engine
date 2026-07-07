#pragma once

#include "EmitData.h"
#include "DirectXGame/engine/Effect/Particle/CPU/ParticleData.h"
#include<random>


namespace Engine {
	namespace EmitFunction {
		// パーティクル生成3d位置
		void CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos);
		// パーティクル生成3d位置+方向
		void CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos, Vector3 direction);
		// パーティクル生成2d
		void CreateParticle(ParticleGroup2d& group, EffectEmitData2d emitData, std::mt19937& randomEngine, Vector3 emitPos);

	}
}


