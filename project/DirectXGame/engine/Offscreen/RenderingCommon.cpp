#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "RenderingCommon.h"

void RenderingCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;	// DX共通クラス

	// 各ポストエフェクト初期化
	copy_.Initialize(dxCommon_,"CopyImage");		
	grayScale_.Initialize(dxCommon_, "GrayScale");
	sepia_.Initialize(dxCommon_,"Sepia");
	vignette_.Initialize(dxCommon_, "Vignette");
	smoothing_.Initialize(dxCommon_, "Smoothing");
	gaussian_.Initialize(dxCommon_, "Gaussian");
	outline_.Initialize(dxCommon_, "Outline");
	radialBlur_.Initialize(dxCommon_, "RadialBlur");
	dissovle_.Initialize(dxCommon_, "Dissovle");
	random_.Initialize(dxCommon_, "Random");
	bloom_.Initialize(dxCommon_, "BloomExtract");
	cimbin_.Initialize(dxCommon_, "BloomCombine");
	
}

void RenderingCommon::DrawRender(PostEffectType type, int index, int indexB)
{
	// 描画方法
	switch (type)
	{
	case PostEffectType::kCopy:	// コピー
		copy_.DrawRender(index, indexB);
		break;
	case PostEffectType::kGrayScale: // グレースケール 
		grayScale_.DrawRender(index, indexB);
		break;
	case PostEffectType::kSepia: // セピア
		sepia_.DrawRender(index, indexB);
		break;
	case PostEffectType::kVignette: // ビネット
		vignette_.DrawRender(index, indexB);
		break;
	case PostEffectType::kSmoothing: // スムージング
		smoothing_.DrawRender(index, indexB);
		break;
	case PostEffectType::kGaussian: // ガウシアンフィルタ
		gaussian_.DrawRender(index, indexB);
		break;
	case PostEffectType::kOitline: // アウトライン
		outline_.DrawRender(index, indexB);
		break;
	case PostEffectType::kRadialBlur: // ラジアルブラ
		radialBlur_.DrawRender(index, indexB);
		break;
	case PostEffectType::kDissovle: // ディゾルブ
		dissovle_.DrawRender(index, indexB);
		break;
	case PostEffectType::kRandom: // ランダム
		random_.DrawRender(index, indexB);
		break;
	case PostEffectType::kBloom: // ブルーム
		bloom_.DrawRender(index, indexB);
		break;
	case PostEffectType::kBloomCombin: // 合成
		cimbin_.DrawRender(indexB,index);
		break;
	default:
		break;
	}

}





