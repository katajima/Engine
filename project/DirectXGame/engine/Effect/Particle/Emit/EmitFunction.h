#pragma once

#include "EmitData.h"
#include "DirectXGame/engine/Effect/Particle/CPU/ParticleData.h"
#include<random>


namespace Engine {
	namespace EmitFunction {
		/// <summary>
		/// パーティクル生成3d位置
		/// </summary>
		void CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos);
		/// <summary>
		/// パーティクル生成3d位置+方向
		/// </summary>
		void CreateParticle(ParticleGroup& group, EffectEmitData emitData, std::mt19937& randomEngine, Vector3 emitPos, Vector3 direction);
		/// <summary>
		/// パーティクル生成2d
		/// </summary>
		void CreateParticle(ParticleGroup2d& group, EffectEmitData2d emitData, std::mt19937& randomEngine, Vector3 emitPos);

	}
}


