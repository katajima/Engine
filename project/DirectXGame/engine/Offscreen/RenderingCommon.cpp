#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "RenderingCommon.h"

void Engine::RenderingCommon::Initialize(DirectXCommon* dxCommon)
{
	this->dxCommon = dxCommon;	// DX共通クラス

	// 各ポストエフェクト初期化
	copy_.Initialize(dxCommon,"CopyImage");		
	grayScale_.Initialize(dxCommon, "GrayScale");
	sepia_.Initialize(dxCommon, "Sepia");
	vignette_.Initialize(dxCommon, "Vignette");
	smoothing_.Initialize(dxCommon, "Smoothing");
	gaussian_.Initialize(dxCommon, "Gaussian");
	outline_.Initialize(dxCommon, "Outline");
	radialBlur_.Initialize(dxCommon, "RadialBlur");
	dissolve_.Initialize(dxCommon, "Dissolve");
	random_.Initialize(dxCommon, "Random");
	bloom_.Initialize(dxCommon, "BloomExtract");
	combine_.Initialize(dxCommon, "BloomCombine");
	
}

void Engine::RenderingCommon::DrawRender(PostEffectType type, int index, int indexB)
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
	case PostEffectType::kDissolve: // ディゾルブ
		dissolve_.DrawRender(index, indexB);
		break;
	case PostEffectType::kRandom: // ランダム
		random_.DrawRender(index, indexB);
		break;
	case PostEffectType::kBloom: // ブルーム
		bloom_.DrawRender(index, indexB);
		break;
	case PostEffectType::kBloomCombine: // 合成
		combine_.DrawRender(indexB,index);
		break;
	default:
		break;
	}

}





