#pragma once
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Matrix4x4.h"

#pragma region GPUData

namespace Engine {
	// ビネット
	struct VignetteGPU {
		float scale;
		float squared;
		float padding[2];
	};

	// スムージング
	struct SmoothigGPU {
		int num;
		float padding[3];
	};

	// ガウシアン
	struct GaussianGPU {
		int num;
		float sigma;
		float padding[2];
	};

	// アウトライン
	struct OutlineGPU {
		Matrix4x4 projectionInverse;
		int num;
		float weightSquared;
		float nearZ;
		float farZ;
	};

	// ラジアルブラ
	struct RadialBlurGPU
	{
		Vector2 center;
		int numSamples;
		float blurWidth;
	};

	// ディゾルブ
	struct DissovleGPU
	{
		float threshold;
		Vector3 color;
		float edge;
		float pad[3];
	};

	// ランダム
	struct RandomGPU
	{
		float time;
		float pad[3];
	};

	// ブルーム
	struct BloomGPU
	{
		float threshold;
		float intensity;
		float pad[2];
	};


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
}


#pragma endregion // GPUに送るデータ



