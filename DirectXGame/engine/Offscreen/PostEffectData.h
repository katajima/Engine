#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"


#pragma region GPUData

struct VignetteGPU {
	float scale;
	float squared;
	float padding[2];
};

struct SmoothigGPU {
	int num;
	float padding[3];
};

struct GaussianGPU {
	int num;
	float sigma;
	float padding[2];
};

struct OutlineGPU {
	Matrix4x4 projectionInverse;
	int num;
	float weightSquared;
	float nearZ;
	float farZ;
};

struct RadialBlurGPU
{
	Vector2 center;
	int numSamples;
	float blurWidth;
};

struct DissovleGPU
{
	float threshold;
	Vector3 color;
	float edge;
	float pad[3];
};

struct RandomGPU
{
	float time;
	float pad[3];
};

struct BloomGPU
{
	float threshold;
	float intensity;
	float pad[2];
};



#pragma endregion // GPUに送るデータ



enum class PostEffectType {
	kCopy,			// コピー
	kGrayScale,		// グレースケール
	kSepia,			// セピア
	kVignette,      // ビネット
	kSmoothing,     // スムージング
	kGaussian,      // ガウス
	kOitline,		// アウトライン
	kRadialBlur,	// ラジアルブラー
	kDissovle,      // ディゾルブ
	kRandom,		// ランダム
	kBloom,			// ブルーム
	kBloomCombin,	// 合成ブルーム
};

enum class PostEffectBlockType {
	kCopy,			// コピー
	kGrayScale,		// グレースケール
	kSepia,			// セピア
	kVignette,      // ビネット
	kSmoothing,     // スムージング
	kGaussian,      // ガウス
	kOitline,		// アウトライン
	kRadialBlur,	// ラジアルブラー
	kDissovle,      // ディゾルブ
	kRandom,		// ランダム
	kBloom,			// ブルーム
	kDoF,			// 被写界深度
};
