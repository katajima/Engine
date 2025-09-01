#pragma once

#include "EmitData.h"
#include "DirectXGame/engine/Effect/Particle/ParticleData.h"
#include<random>

namespace EmitFanction {
	// エミッターの形状によっての出る位置の設定
	Vector3 EmitPos(ParticleGroup& particleGroup, WorldTransform& transform,EmitData::SpawnType emitType, std::mt19937& randomEngine, int index = 0);
	// 
	void CreateParticle(ParticleGroup& grop, WorldTransform& transform, std::mt19937& randomEngine, EmitData::SpawnType spawnType, int index = 0);
}


