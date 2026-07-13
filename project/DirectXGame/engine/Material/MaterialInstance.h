#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Color/Color.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


//// GPUに送るデータ
//struct DataGPU
//{
//	Color color;
//	int32_t enableLighting;
//	float alphaClipping = 0.5f;
//	float environmentCoefficient = 0.5f;
//	float alpha = 1.0f;
//	Matrix4x4 uvTransform;
//	float shininess;
//	int32_t useLig;
//	int32_t useHim;
//	int32_t useNormalMap;
//	int32_t useSpecularMap;
//	float padding2[3];
//};

struct MaterialInstance {
	Transform transform;
	Color color;
	int32_t enableLighting_ = false;
	float  environmentCoefficient_ = 0.5f;
	float shininess_ = 64.0f;
	int32_t useLig_ = false;
	int32_t useNormalMap_ = 0;
	int32_t useSpecularMap_ = 0;
	bool useEnvironment_ = 0;
	float alphaClipping_ = 0.5f;
	float alpha_ = 1.0f;
};


