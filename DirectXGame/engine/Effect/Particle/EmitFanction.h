#pragma once

#include "ParticleData.h"
#include<random>

namespace EmitFanction {

	// ランダムエンジンを使った乱数生成(float)
	float RandFloat(const MaxMin<float>& vec3, std::mt19937& randomEngine);

	// ランダムエンジンを使った乱数生成(Vector2)
	Vector2 RandVector2(const MaxMin<Vector2>& vec3, std::mt19937& randomEngine);

	// ランダムエンジンを使った乱数生成(Vector3)
	Vector3 RandVector3(const MaxMin<Vector3>& vec3, std::mt19937& randomEngine);

	// ランダムエンジンを使った乱数生成(Vector4)
	Vector4 RandVector4(const MaxMin<Vector4>& vec3, std::mt19937& randomEngine);

	// エミッターの形状によっての出る位置の設定
	Vector3 EmitPos(ParticleGroup& particleGroup, ParticleData::SpawnType emitType, std::mt19937& randomEngine, int index = 0);

	//
	void ConversionMinMaxFloat(MaxMin<float>& renge);

	//
	void ConversionMinMaxV2(MaxMin<Vector2>& renge);

	//
	void ConversionMinMaxV3(MaxMin<Vector3>& renge);

	//
	void ConversionMinMaxV4(MaxMin<Vector4>& renge);


}


